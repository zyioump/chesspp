#ifndef Board_H
#define Board_H

#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdint>
#include <map>
#include <algorithm>
#include <random>
#include <limits>
#include <filesystem>

#include "utils.hpp"
#include "zobrist.hpp"

extern uint64_t bishopLegalAttack[64][512];
extern uint64_t rookLegalAttack[64][4096];
extern uint64_t knightAttack[64];
extern uint64_t kingAttack[64];
extern uint64_t pawnAttack[2][64];
extern uint64_t pawnMove[2][64];
extern Magic bishopMagic[64];
extern Magic rookMagic[64];

extern uint64_t powOf2[64];

class Board {
    private:
        void initAttack();
        void initRookAttack();
        void initKnightAttack();
        void initKingAttack();
        void initPawnAttack();

        void initSlidingPieces();
        /* void initSlidingPiece(bool bishop); */
        void find_magic(int square, bool isBishop);

        uint64_t getBishopAttackMask(int square);
        uint64_t getBishopLegalAttack(int square, uint64_t occupancy);
        uint64_t getRookAttackMask(int square);
        uint64_t getRookLegalAttack(int square, uint64_t occupancy);
        uint64_t getSlidingPieceLegalAttack(int square, uint64_t occupancyBitboard, bool isBishop);
        int computeMagicKey(uint64_t magic, uint64_t occ, int shift);

        void generateMoves();
        EnemyAttack getEnemyAttackBitboard(uint64_t occupancy);
        void findPinnedMask(int square, uint64_t pieceAttack, uint64_t kingPieceAttack, uint64_t occupancy, bool isBishop, std::vector<PinnedPiece>* pinnedPieces);

        Zobrist zobristHelper;
    public:
        std::vector<Move> legalMoves;
        uint64_t enemyAttacks[6];

        uint64_t piecesBitboards[2][6] = {0};
        bool inCheck = false;
        int enPassantCol = -1;
        bool castling[2][2] = {0};
        Color turn = White;

        std::vector<BoardSnapshot> moveStack;

        uint64_t zobrist;

        bool push(Move move);
        void pop();

        bool isRepetition();
        uint64_t getColorBitboard(Color color);
        std::vector<int> pieces(PieceType pieceType, Color color);
        bool pieceAt(uint square, Piece* piece);
        bool pieceAtBitboard(uint64_t bitboard, Piece* piece);
        Board(std::string fen);
};
#endif
