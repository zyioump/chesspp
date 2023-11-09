#ifndef Board_H
#define Board_H

#include <string>
#include <iostream>
#include <bitset>
#include <math.h>
#include <unordered_set>
#include <cstdint>
#include <map>
#include <list>

#include <utils.hpp>

class Board {
    private:
        uint64_t piecesBitboards[2][6] = {0};
        Color turn = White;
        bool castle[2][2] = {0};
        int enPassantCol = -1;

        std::list<Move> moveStack;

        std::unordered_set<Direction> queenDirections = {No, So, Ea, We, NoEa, NoWe, SoEa, SoWe};
        std::unordered_set<Direction> rookDirections = {No, So, Ea, We};
        std::unordered_set<Direction> bishopDirections = {NoEa, NoWe, SoEa, SoWe};

        std::unordered_set<PinnedPiece, PinnedPieceHash> pinnedPiece;

        void generateMoves();
        std::unordered_set<Move, MoveHash> generateColorMoves(uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t globalBitboard, Color color, std::unordered_set<Move, MoveHash>* opponentLegalAttackPtr);

        std::unordered_set<Move, MoveHash> generatePawnMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color);
        std::unordered_set<Move, MoveHash> generateKnightMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color);
        std::unordered_set<Move, MoveHash> generateKingMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t attackedSquare, Color color);
        std::unordered_set<Move, MoveHash> generateSlidingPiecesMoves(uint64_t bitboard, std::unordered_set<Direction> rayDirections, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color);
        
    public:
        std::unordered_set<Move, MoveHash> legalMoves;
        std::unordered_set<Move, MoveHash> opponentLegalMoves;
        bool inCheck = false;

        bool push(Move move);

        uint64_t getColorBitboard(Color color);
        std::unordered_set<uint64_t> pieces(PieceType pieceType, Color color);
        bool pieceAt(uint square, Piece* piece);
        bool pieceAtBitboard(uint64_t bitboard, Piece* piece);
        Board(std::string fen);
        Board();
};
#endif
