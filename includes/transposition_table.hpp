#ifndef Transposition_table_H
#define Transposition_table_H

#include "utils.hpp"

enum TTFlag {
    EXACT,
    ALPHA,
    BETA
};

struct TTEntry {
    Move bestMove;
    int depth;
    uint64_t zobrist;
    int score;
    TTFlag flag;
};

class TranspositionTable {
    private:
        static const int tableSize = 1e8;
        TTEntry* table = new TTEntry[tableSize];
    public:
        TranspositionTable();
        void setEntry(uint64_t zobrist, Move bestMove, int depth, int score, TTFlag flag);
        bool getEntry(uint64_t zobrist, TTEntry* entry);
        void freeMemory();
};

#endif
