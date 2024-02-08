#include "utils.hpp"

uint64_t random_uint64() {
  uint64_t u1, u2, u3, u4;
  u1 = (uint64_t)(random()) & 0xFFFF; u2 = (uint64_t)(random()) & 0xFFFF;
  u3 = (uint64_t)(random()) & 0xFFFF; u4 = (uint64_t)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64_t random_uint64_fewbits() {
  return random_uint64() & random_uint64() & random_uint64();
}

uint64_t noHCol = 0b0111111101111111011111110111111101111111011111110111111101111111;
uint64_t noACol = 0b1111111011111110111111101111111011111110111111101111111011111110;
uint64_t noGHCol = 0b0011111100111111001111110011111100111111001111110011111100111111;
uint64_t noABCol = 0b1111110011111100111111001111110011111100111111001111110011111100;
uint64_t no8Row = 0b11111111111111111111111111111111111111111111111111111111;
uint64_t no1Row = 0xffffffffffffff00;

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
uint64_t northOne(uint64_t bitboard) { return (bitboard & no8Row) << 8; }
uint64_t southOne(uint64_t bitboard) { return (bitboard & no1Row) >> 8; }

std::vector<int> serializeBitboard(uint64_t bitboard) {
    std::vector<int> squares;

    int square;
    if (bitboard) do {
        square = forwardBitscan(bitboard);
        squares.push_back(square);
    } while (bitboard &= bitboard-1);

    return squares;
}

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
uint forwardBitscan(uint64_t bitboard) {return reverseBitscan(bitboard & -bitboard);}

struct Move createMove(uint64_t from, uint64_t to, bool promotion, bool castling, bool enPassant) {
    Move move;
    move.to = to;
    move.from = from;
    move.promotion = promotion;
    move.castling = castling;
    move.enPassant = enPassant;
    return move;
}

std::string bitboardToSquareName(uint64_t bitboard) {
    uint square = reverseBitscan(bitboard);
    uint col = square & 7;
    uint row = (square - col) / 8;

    std::ostringstream squareStream;
    squareStream << char ('A' + col) << char ('1' + row);

    return squareStream.str();
}

int count1(uint64_t bitboard) {
    int num;
    for (num=0; bitboard; num++, bitboard &= bitboard-1);
    return num;
}
