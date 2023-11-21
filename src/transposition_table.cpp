#include "transposition_table.hpp"

TranspositionTable::TranspositionTable() {}

void TranspositionTable::setEntry(uint64_t zobrist, Move bestMove, int depth, int score, TTFlag flag) {
    TTEntry entry;
    entry.zobrist = zobrist;
    entry.bestMove = bestMove;
    entry.depth = depth;
    entry.score = score;
    entry.flag = flag;

    table[zobrist % tableSize] = entry;
}

bool TranspositionTable::getEntry(uint64_t zobrist, TTEntry* outputEntry) {
    TTEntry entry = table[zobrist % tableSize];
    if (entry.zobrist == zobrist) {
        *outputEntry = entry;
        return true;
    }
    return false;
}

void TranspositionTable::freeMemory() {
    delete[] table;
}
