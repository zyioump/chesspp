#include "board.hpp"

/* #define USE_32_BIT_MULTIPLICATIONS */

uint64_t Board::getBishopAttackMask(int square) {
    uint64_t bitboard = 0;
    int rank, file;
    int sFile = square & 7;
    int sRank = square >> 3;

    for (file=sFile+1, rank=sRank+1; file < 7 && rank < 7; rank++, file++) bitboard |= powOf2[rank*8 + file];
    for (file=sFile-1, rank=sRank+1; file >= 1 && rank < 7; rank++, file--) bitboard |= powOf2[rank*8 + file];
    for (file=sFile+1, rank=sRank-1; file < 7 && rank >= 1; rank--, file++) bitboard |= powOf2[rank*8 + file];
    for (file=sFile-1, rank=sRank-1; file >= 1 && rank >= 1; rank--, file--) bitboard |= powOf2[rank*8 + file];

    return bitboard;
}

uint64_t Board::getRookAttackMask(int square) {
    uint64_t bitboard = 0;
    int rank, file;
    int sFile = square & 7;
    int sRank = square >> 3;

    for (file=sFile+1, rank=sRank; file < 7; file++) bitboard |= powOf2[rank*8 + file];
    for (file=sFile-1, rank=sRank; file > 0; file--) bitboard |= powOf2[rank*8 + file];
    for (rank=sRank-1, file=sFile; rank > 0; rank--) bitboard |= powOf2[rank*8 + file];
    for (rank=sRank+1, file=sFile; rank < 7; rank++) bitboard |= powOf2[rank*8 + file];

    return bitboard;
}

uint64_t Board::getBishopLegalAttack(int square, uint64_t occupancy) {
    uint64_t bitboard = 0;
    int rank, file;
    int sFile = square & 7;
    int sRank = square >> 3;

    for (file=sFile+1, rank=sRank+1; file < 8 && rank < 8; rank++, file++){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }
    for (file=sFile-1, rank=sRank+1; file >= 0 && rank < 8; rank++, file--){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }
    for (file=sFile+1, rank=sRank-1; file < 8 && rank >= 0; rank--, file++){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }
    for (file=sFile-1, rank=sRank-1; file >= 0 && rank >= 0; rank--, file--){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }

    return bitboard;
}

uint64_t Board::getRookLegalAttack(int square, uint64_t occupancy) {
    uint64_t bitboard = 0;
    int rank, file;
    int sFile = square & 7;
    int sRank = square >> 3;

    for (file=sFile+1, rank=sRank; file < 8; file++){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }
    for (file=sFile-1, rank=sRank; file >= 0; file--){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }
    for (rank=sRank-1, file=sFile; rank >= 0; rank--){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }
    for (rank=sRank+1, file=sFile; rank < 8; rank++){
        bitboard |= powOf2[rank*8 + file];
        if (occupancy & powOf2[rank*8 + file]) break;
    }

    return bitboard;
}

void Board::initSlidingPieces() {
    if (std::filesystem::exists("res/magic.numbers")) {
        FILE *magicFileRd = fopen("res/magic.numbers", "r");
        size_t bytesReaded;
        bytesReaded = fread(&bishopMagic, sizeof(Magic), 64, magicFileRd);
        if (bytesReaded == 64) {
            bytesReaded = fread(&rookMagic, sizeof(Magic), 64, magicFileRd);
            if (bytesReaded == 64) {
                bytesReaded = fread(&bishopLegalAttack, sizeof(uint64_t), 64*512, magicFileRd);
                if (bytesReaded == 64*512) {
                    bytesReaded = fread(&rookLegalAttack, sizeof(uint64_t), 64*4096, magicFileRd);
                    if (bytesReaded == 64*4096) {
                        fclose(magicFileRd);
                        return;
                    }
                }
            }
        }
        fclose(magicFileRd);
    }

    printf("Cannot find existing magic numbers, generating new ones...\n");
    for (int square=0; square<64; square++) {
        find_magic(square, true);
        find_magic(square, false);
    }

    FILE *magicFileWr = fopen("res/magic.numbers", "w");
    fwrite(&bishopMagic, sizeof(Magic), 64, magicFileWr);
    fwrite(&rookMagic, sizeof(Magic), 64, magicFileWr);
    fwrite(&bishopLegalAttack, sizeof(uint64_t), 64*512, magicFileWr);
    fwrite(&rookLegalAttack, sizeof(uint64_t), 64*4096, magicFileWr);
    fclose(magicFileWr);

    printf("Saved to res/magic.numbers\n");
}

uint64_t Board::getSlidingPieceLegalAttack(int square, uint64_t occupancyBitboard, bool isBishop) {
    if (isBishop) {
        Magic magic = bishopMagic[square];
        uint64_t blockerBitboard = occupancyBitboard & magic.mask;
        uint64_t key = computeMagicKey(magic.magic, blockerBitboard, magic.shift);
        return bishopLegalAttack[square][key];
    }
    Magic magic = rookMagic[square];
    uint64_t blockerBitboard = occupancyBitboard & magic.mask;
    uint64_t key = computeMagicKey(magic.magic, blockerBitboard, magic.shift);
    return rookLegalAttack[square][key];
}

void Board::find_magic(int square, bool isBishop){
    int maxShift = (isBishop) ? 9 : 12;
    uint64_t occ[powOf2[maxShift]];
    uint64_t att[powOf2[maxShift]];
    uint64_t magicAtt[powOf2[maxShift]];

    uint64_t mask = (isBishop) ? getBishopAttackMask(square) : getRookAttackMask(square);

    auto maskSquares = serializeBitboard(mask);
    int shift = maskSquares.size();

    for (int i=0; i<powOf2[shift]; i++) {
        uint64_t occupancy = 0;
        for (int bitI=0; bitI < shift; bitI++) {
            int bit = (i>>bitI) & 1;
            occupancy |= (uint64_t) bit << maskSquares[bitI];
        }
        occ[i] = occupancy;
        att[i] = (isBishop) ? getBishopLegalAttack(square, occupancy) : getRookLegalAttack(square, occupancy);
    }

    uint64_t magicNum;
    int fail = 0;
    while (true) {
        magicNum = random_uint64_fewbits();
        fail = 0;
        for (int i=0; i<powOf2[maxShift]; i++) magicAtt[i] = 0ULL;
        for (int i=0; i<powOf2[shift]; i++) {
            int key = computeMagicKey(magicNum, occ[i], shift);
            if (magicAtt[key] == 0ULL)
                magicAtt[key] = att[i];
            else if(magicAtt[key] != att[i]) {
                fail=1;
                break;
            }
        }
        if (!fail) break;
    }

    std::copy(magicAtt, magicAtt + powOf2[maxShift], (isBishop) ? bishopLegalAttack[square] : rookLegalAttack[square]);

    Magic magic;
    magic.mask = mask;
    magic.magic = magicNum;
    magic.shift = shift;
    if (isBishop) bishopMagic[square] = magic;
    else rookMagic[square] = magic;
}

int Board::computeMagicKey(uint64_t magic, uint64_t occ, int shift) {
#if defined(USE_32_BIT_MULTIPLICATIONS)
    return
        (unsigned)((int)occ*(int)magic ^ (int)(occ>>32)*(int)(magic>>32)) >> (32-shift);
#else
    return (int)((occ * magic) >> (64 - shift));
#endif
}
