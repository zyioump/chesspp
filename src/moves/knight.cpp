#include <board.hpp>


std::vector<Move> Board::generateKnightMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t oppentColorBitboard, Color color) {
    int square = reverseBitscan(bitboard);
    std::vector<Move> moves = knightAttack[square];

    for (int i=0; i<moves.size(); i++) {
        if (moves[i].to & colorBitboard) moves[i].defend = true;
    }
    return moves;
} 
