#include "utils.hpp"

uint64_t noHCol = 0b0111111101111111011111110111111101111111011111110111111101111111;
uint64_t noACol = 0b1111111011111110111111101111111011111110111111101111111011111110;
uint64_t noGHCol = 0b0011111100111111001111110011111100111111001111110011111100111111;
uint64_t noABCol = 0b1111110011111100111111001111110011111100111111001111110011111100;
uint64_t no1Row = 0b0000000000000000000000000000000000000000000000000000000011111111;
uint64_t no8Row = 0b1111111100000000000000000000000000000000000000000000000000000000;

void displayBitboard(uint64_t bitboard) {
    uint mask = 0b11111111;
    for (int row=7; row>=0; row--) {
        uint rowBitboard = (bitboard >> 8*row) & mask;
        for (int col=0; col<8; col++) {
            printf("%u", (rowBitboard >> col) & 1);
        }
        printf("\n");
    }
    printf("\n");
}

uint64_t eastOne(uint64_t bitboard) { return (bitboard & noHCol) << 1; }
uint64_t noEaOne(uint64_t bitboard) { return (bitboard & noHCol) << 9; }
uint64_t soEaOne(uint64_t bitboard) { return (bitboard & noHCol) >> 7; }
uint64_t westOne(uint64_t bitboard) { return (bitboard & noACol) >> 1; }
uint64_t soWeOne(uint64_t bitboard) { return (bitboard & noACol) >> 9; }
uint64_t noWeOne(uint64_t bitboard) { return (bitboard & noACol) << 7; }

std::unordered_set<uint64_t> serializeBitboard(uint64_t bitboard) {
    std::unordered_set<uint64_t> serializedBitboards;

    while (bitboard) {
        uint64_t seriablizedBitboard = bitboard & -bitboard;
        serializedBitboards.insert(seriablizedBitboard);
        bitboard &= bitboard - 1;
    }

    return serializedBitboards;
}

uint64_t rayAttackBitboard[64][8] = {0};

void initRayAttack(){
    std::map<Direction, uint64_t> directionLookup;

    directionLookup[No] =   0b100000001000000010000000100000001000000010000000100000000;
    directionLookup[NoEa] = 0b1000000001000000001000000001000000001000000001000000001000000000;
    directionLookup[NoWe] = 0b100000010000001000000100000010000001000000100000000000000;
    directionLookup[Ea] =   0b11111110;
    directionLookup[So] =   0b1000000010000000100000001000000010000000100000001;
    directionLookup[SoEa] = 0b10000001000000100000010000001000000100000010000000;
    directionLookup[SoWe] = 0b1000000001000000001000000001000000001000000001000000001;
    directionLookup[We] =   0b111111100000000000000000000000000000000000000000000000000000000;

    for (int direction=0; direction<8; direction++) {
        /* if (direction != So) continue; */
        uint64_t lookup = directionLookup[(Direction) direction];

        if (direction == No || direction == Ea || direction == NoWe || direction == NoEa) {
            if (direction == NoWe) {
                for (int col=7; col >= 0; col--, lookup = westOne(lookup)) {
                    uint64_t colLookup = lookup;
                    for (int row8=0; row8 < 64; row8 += 8, colLookup <<= 8) {
                        rayAttackBitboard[row8 + col][direction] = colLookup;
                    }
                }
            } else {
                for (int col=0; col < 8; col++, lookup = eastOne(lookup)) {
                    uint64_t colLookup = lookup;
                    for (int row8=0; row8 < 64; row8 += 8, colLookup <<= 8) {
                        rayAttackBitboard[row8 + col][direction] = colLookup;
                    }
                }
            }
        } else {
            if (direction == SoWe || direction == We) {
                for (int col=7; col >= 0; col--, lookup = westOne(lookup)) {
                    uint64_t colLookup = lookup;
                    for (int row8=56; row8 >= 0; row8 -= 8, colLookup >>= 8) {
                        rayAttackBitboard[row8 + col][direction] = colLookup;
                    }
                }
            } else {
                for (int col=0; col < 8; col++, lookup = eastOne(lookup)) {
                    uint64_t colLookup = lookup;
                    for (int row8=56; row8 >= 0; row8 -= 8, colLookup >>= 8) {
                        rayAttackBitboard[row8 + col][direction] = colLookup;
                    }
                }

            }
        }
    }
}

uint forwardBitscan(uint64_t bitboard) {return log2(bitboard & -bitboard);}
uint reverseBitscan(uint64_t bitboard) {
    uint result = 0;
    if (bitboard > 0xFFFFFFFF) {
        bitboard >>= 32;
        result += 32;
    }
    if (bitboard > 0xFFFF) {
        bitboard >>= 16;
        result += 16;
    }
    if (bitboard > 0xFF) {
        bitboard >>= 8;
        result += 8;
    }
    if (bitboard > 0xF) {
        bitboard >>= 4;
        result += 4;
    }
    if (bitboard > 0x3) {
        bitboard >>= 2;
        result += 2;
    }
    if (bitboard > 0x1) {
        bitboard >>= 1;
        result += 1;
    }
    return result;
}

struct Move createMove(uint64_t from, uint64_t to, bool promotion, bool attack, bool defend) {
    Move move;
    move.to = to;
    move.from = from;
    move.promotion = promotion;
    move.attack = attack;
    move.defend = defend;
    return move;
}

uint64_t rowMask(int square) {
    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000011111111;
    mask <<= square & 56;
    return mask;
}

uint64_t colMask(int square) {
    uint64_t mask = 0b0000000100000001000000010000000100000001000000010000000100000001;
    mask <<= square & 7;
    return mask;
}

uint64_t diagonalMask(int square) {
    uint64_t mainDiagonal = 0b1000000001000000001000000001000000001000000001000000001000000001;
    int diag = 8*(square & 7) - (square & 56);
    int north = -diag & (diag >> 31);
    int south = diag & (-diag >> 31);
    return (mainDiagonal >> south) << north;
}

uint64_t antiDiagonalMask(int square) {
    uint64_t mainDiagonal = 0b100000010000001000000100000010000001000000100000010000000;
    int diag = 56 - 8*(square & 7) - (square & 56);
    int north = -diag & (diag >> 31);
    int south = diag & (-diag >> 31);
    return (mainDiagonal >> south) << north;
}
