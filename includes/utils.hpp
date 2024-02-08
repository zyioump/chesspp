#ifndef Utils_h
#define Utils_h

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>

enum PieceType {
    King,
    Queen,
    Bishop,
    Knight,
    Rook,
    Pawn 
};

enum Side {
    QueenSide,
    KingSide
};

enum Color {
    White,
    Black
};

struct Move {
    uint64_t from;
    uint64_t to;
    bool promotion;
    bool castling;
    bool enPassant;
    // Should be ok to ignore attack, defend and promotion for equality or hashing
    // Not sure
    bool operator==(const Move move) const {return from == move.from && to == move.to;};
    Move operator=(const Move move) {
        from = move.from;
        to = move.to;
        promotion = move.promotion;
        castling = move.castling;
        enPassant = move.enPassant;
        return *this;
    }
};

struct Piece {
    Color color;
    PieceType pieceType;
};

struct PinnedPiece {
    int square;
    uint64_t mask;
    uint64_t attacker;
    bool operator==(const int cmpSquare) const { return square == cmpSquare; }
};

struct Magic {
    uint64_t mask;
    uint64_t magic;
    int shift;
};

struct EnemyAttack {
    uint64_t attack;
    uint64_t captureProtect;
    uint64_t moveProtect;
    int checkNum;
    std::vector<PinnedPiece> pinnedPieces;
};

struct BoardSnapshot {
    Move move;
    bool castling[2][2];
    Color turn;
    int enPassantCol;
    uint64_t piecesBitboards[2][6];
    std::vector<Move> legalMoves;
    uint64_t zobrist;
    bool lastMoveIsCapture;
};

extern uint64_t noHCol;
extern uint64_t noACol;
extern uint64_t noGHCol;
extern uint64_t noABCol;
extern uint64_t no1Row;
extern uint64_t no8Row;

void displayBitboard(uint64_t bitboard);

struct Move createMove(uint64_t from, uint64_t to, bool promotion, bool castling, bool enPassant);

uint64_t eastOne(uint64_t bitboard);
uint64_t noEaOne(uint64_t bitboard);
uint64_t soEaOne(uint64_t bitboard);
uint64_t westOne(uint64_t bitboard);
uint64_t soWeOne(uint64_t bitboard);
uint64_t noWeOne(uint64_t bitboard);
uint64_t northOne(uint64_t bitboard);
uint64_t southOne(uint64_t bitboard);

int count1(uint64_t bitboard);

std::vector<int> serializeBitboard(uint64_t bitboard);
uint forwardBitscan(uint64_t bitboard);
uint reverseBitscan(uint64_t bitboard);

std::string bitboardToSquareName(uint64_t bitboard);

uint64_t random_uint64();
uint64_t random_uint64_fewbits();
#endif
