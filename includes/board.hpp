#ifndef Board_H
#define Board_H

#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>

#include "utils.hpp"
#include "zobrist.hpp"

class Board {
    private:

        std::vector<Direction> queenDirections = {No, So, Ea, We, NoEa, NoWe, SoEa, SoWe};
        std::vector<Direction> rookDirections = {No, So, Ea, We};
        std::vector<Direction> bishopDirections = {NoEa, NoWe, SoEa, SoWe};

        std::vector<PinnedPiece> pinnedPieces;

        void generateMoves();
        std::vector<Move> generateColorMoves(uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t globalBitboard, Color color, std::vector<Move>* opponentLegalAttackPtr);

        std::vector<Move> removeNonKingProtectionMove(std::vector<Move> moves, uint64_t piecesBitboard);
        std::vector<Move> removeNonLegalMove(std::vector<Move> moves, uint64_t kingProtectionSquare, PieceType pieceType);

        std::vector<Move> generatePawnMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color);
        std::vector<Move> generateKnightMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color);
        std::vector<Move> generateKingMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t attackedSquare, Direction attackerDirection, Color color);
        std::vector<Move> generateSlidingPiecesMoves(uint64_t bitboard, std::vector<Direction> rayDirections, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color);
        Zobrist zobristHelper;

    public:
        std::vector<Move> legalMoves;
        std::vector<Move> opponentLegalMoves;

        uint64_t piecesBitboards[2][6] = {0};
        bool inCheck = false;
        int enPassantCol = -1;
        bool castling[2][2] = {0};
        Color turn = White;

        std::vector<BoardSnapshot> moveStack;

        uint64_t zobrist;

        bool push(Move move);
        void pop();

        uint64_t getColorBitboard(Color color);
        std::vector<uint64_t> pieces(PieceType pieceType, Color color);
        bool pieceAt(uint square, Piece* piece);
        bool pieceAtBitboard(uint64_t bitboard, Piece* piece);
        Board(std::string fen);
};
#endif
