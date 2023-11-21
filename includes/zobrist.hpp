#ifndef Zobrist_H
#define Zobrist_H

#include <random>
#include <limits>

#include "utils.hpp"

class Zobrist{
    private:
        uint64_t piecesZobrist[2][6][64];
        uint64_t castlingZobrist[2][4];
        uint64_t enPassantZobrist[16];
        uint64_t sideZobrist;

    public:
        Zobrist();
        uint64_t getZobrist(uint64_t piecesBitboards[2][6], bool castling[2][2], int enPassantCol, Color color);
        uint64_t updateZobrist(uint64_t zobrist, bool oldCastling[2][2], int oldEnPassantCol, bool castling[2][2], int enPassantCol, Piece fromPiece, Piece toPiece, bool toPieceExist, Color newColor, Move move);
};

#endif
