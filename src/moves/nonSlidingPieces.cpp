#include <board.hpp>

void Board::initKingAttack() {
    for (int square=0; square<64; square++) {
        uint64_t bitboard = powOf2[square];
        kingAttack[square] ^= northOne(bitboard);
        kingAttack[square] ^= southOne(bitboard);
        kingAttack[square] ^= westOne(bitboard);
        kingAttack[square] ^= eastOne(bitboard);
        kingAttack[square] ^= noEaOne(bitboard);
        kingAttack[square] ^= soEaOne(bitboard);
        kingAttack[square] ^= noWeOne(bitboard);
        kingAttack[square] ^= soWeOne(bitboard);
    }
}

void Board::initKnightAttack() {
    for (int square=0; square < 64; square++) {
        uint64_t bitboard = powOf2[square];
        knightAttack[square] ^= (bitboard & noHCol) << 17;
        knightAttack[square] ^= (bitboard & noGHCol) << 10;
        knightAttack[square] ^= (bitboard & noGHCol) >> 6;
        knightAttack[square] ^= (bitboard & noHCol) >> 15;
        knightAttack[square] ^= (bitboard & noACol) << 15;
        knightAttack[square] ^= (bitboard & noABCol) << 6;
        knightAttack[square] ^= (bitboard & noABCol) >> 10;
        knightAttack[square] ^= (bitboard & noACol) >> 17;
    }
}

void Board::initPawnAttack() {
    for (int square=0; square<64; square++) {
        uint64_t bitboard = powOf2[square];
        pawnMove[White][square] ^= northOne(bitboard);
        pawnMove[Black][square] ^= southOne(bitboard);

        if (square >> 3 == 1) pawnMove[White][square] ^= northOne(northOne(bitboard));
        if (square >> 3 == 6) pawnMove[Black][square] ^= southOne(southOne(bitboard));

        pawnAttack[White][square] ^= noWeOne(bitboard);
        pawnAttack[White][square] ^= noEaOne(bitboard);
        pawnAttack[Black][square] ^= soEaOne(bitboard);
        pawnAttack[Black][square] ^= soWeOne(bitboard);
    }
}

