#ifndef Ai_H
#define Ai_H

#include <limits>
#include <chrono>
#include <list>
#include "board.hpp"
#include "transposition_table.hpp"

struct PieceBonus{
    int earlyBonus = 0;
    int endBonus = 0;
    int endGame = 0;
};

class Ai {
    private:
        int negaMax(Board board, int alpha, int beta, int depth, Move* bestMovePtr, std::list<Move>* bestVariation, std::list<Move>* lastVariation);
        int quiesce(Board board, int alpha, int beta);
        int see(Board board, uint64_t bitboard, int pieceValue);
        int seeCapture(Board board, Move move);

        std::vector<std::pair<Move, int>> orderMove(Board board, std::vector<Move> moves, Move* lastBestMove, Move* hashMove);

        int getPieceValue(PieceType pieceType);
        TranspositionTable transpositionTable;
        void cleanMetrics();
        PieceBonus getPieceBonus(PieceType pieceType, Color color, std::vector<uint64_t> pieceBitboards);
        int transposeSquareForBonus(int square, Color color);


        const static int pawnSquareEarlyBonus[64];
        const static int knightSquareEarlyBonus[64];
        const static int bishopSquareEarlyBonus[64];
        const static int rookSquareEarlyBonus[64];
        const static int queenSquareEarlyBonus[64];
        const static int kingSquareEarlyBonus[64];

        const static int pawnSquareEndBonus[64];
        const static int knightSquareEndBonus[64];
        const static int bishopSquareEndBonus[64];
        const static int rookSquareEndBonus[64];
        const static int queenSquareEndBonus[64];
        const static int kingSquareEndBonus[64];

    public:
        const int maxDepth = 5;
        const int timeLimit = 30;
        const int aspirationWindow = getPieceValue(Pawn)/4;
        std::chrono::high_resolution_clock::time_point moveStartTime;
        bool stopSearching = false;
        bool lock = false;
        Move play(Board board);
        std::pair<int, int> evaluate(Board board);
        void freeMemory();
        std::map<std::string, float> metrics;
};
#endif
