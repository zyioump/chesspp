#ifndef Utils_h
#define Utils_h

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
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

// Carefully order so complementary is +- 4
enum Direction {
    NoWe,
    No,
    NoEa,
    Ea,
    SoEa,
    So,
    SoWe,
    We,
    KnightDir,
    NoDirection
};

enum Color {
    White,
    Black
};

struct Move {
    uint64_t from;
    uint64_t to;
    bool attack;
    bool promotion;
    bool defend;
    bool castling;
    bool enPassant;
    Direction direction;
    // Should be ok to ignore attack, defend and promotion for equality or hashing
    // Not sure
    bool operator==(const Move move) const {return from == move.from && to == move.to;};
    Move operator=(const Move move) {
        from = move.from;
        to = move.to;
        attack = move.attack;
        promotion = move.promotion;
        defend = move.defend;
        castling = move.castling;
        enPassant = move.enPassant;
        direction = move.direction;
        return *this;
    }
};

struct Piece {
    Color color;
    PieceType pieceType;
};

extern uint64_t rayAttackBitboard[64][8];
void initRayAttack();

struct PinnedPiece {
    uint square;
    Direction direction;
    
    // Don't check direction in == operator
    // Direction is check after in the pieces move generation
    // Allow to find bitboard without carring about the direction
    bool operator==(const PinnedPiece pinnedPiece) const {return square == pinnedPiece.square;};
};

struct BoardSnapshot {
    Move move;
    bool castling[2][2];
    Color turn;
    int enPassantCol;
    uint64_t piecesBitboards[2][6];
    std::vector<Move> legalMoves;
    std::vector<Move> opponentLegalMoves;
    uint64_t zobrist;
};

extern uint64_t noHCol;
extern uint64_t noACol;
extern uint64_t noGHCol;
extern uint64_t noABCol;
extern uint64_t no1Row;
extern uint64_t no8Row;

void displayBitboard(uint64_t bitboard);

struct Move createMove(uint64_t to, uint64_t from, bool promotion, bool attack, bool defend, bool castling, bool enPassant, Direction direction);

uint64_t eastOne(uint64_t bitboard);
uint64_t noEaOne(uint64_t bitboard);
uint64_t soEaOne(uint64_t bitboard);
uint64_t westOne(uint64_t bitboard);
uint64_t soWeOne(uint64_t bitboard);
uint64_t noWeOne(uint64_t bitboard);

uint64_t rowMask(int square);
uint64_t colMask(int square);

uint64_t diagonalMask(int square);
uint64_t antiDiagonalMask(int square);

std::vector<uint64_t> serializeBitboard(uint64_t bitboard);
uint forwardBitscan(uint64_t bitboard);
uint reverseBitscan(uint64_t bitboard);

std::string bitboardToSquareName(uint64_t bitboard);
#endif
