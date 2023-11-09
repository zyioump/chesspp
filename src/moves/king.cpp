#include <board.hpp>

std::unordered_set<Move, MoveHash> Board::generateKingMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t attackedSquare, Color color) {
    std::unordered_set<Move, MoveHash> moves;
    uint64_t ea = eastOne(bitboard) & ~attackedSquare;
    uint64_t we = westOne(bitboard) & ~attackedSquare;
    uint64_t no = (bitboard << 8) & ~attackedSquare;
    uint64_t so = (bitboard >> 8) & ~attackedSquare;
    uint64_t noEa = noEaOne(bitboard) & ~attackedSquare;
    uint64_t noWe = noWeOne(bitboard) & ~attackedSquare;
    uint64_t soEa = soEaOne(bitboard) & ~attackedSquare;
    uint64_t soWe = soWeOne(bitboard) & ~attackedSquare;

    if (ea != 0)
        if (ea & colorBitboard) moves.insert(createMove(bitboard, ea, false, true, true));
        else moves.insert(createMove(bitboard, ea, false, true, false));
    if (we != 0)
        if (we & colorBitboard) moves.insert(createMove(bitboard, we, false, true, true));
        else moves.insert(createMove(bitboard, we, false, true, false));
    if (no != 0)
        if (no & colorBitboard) moves.insert(createMove(bitboard, no, false, true, true));
        else moves.insert(createMove(bitboard, no, false, true, false));
    if (so != 0)
        if (so & colorBitboard) moves.insert(createMove(bitboard, so, false, true, true));
        else moves.insert(createMove(bitboard, so, false, true, false));
    if (noEa != 0)
        if (noEa & colorBitboard) moves.insert(createMove(bitboard, noEa, false, true, true));
        else moves.insert(createMove(bitboard, noEa, false, true, false));
    if (noWe != 0)
        if (noWe & colorBitboard) moves.insert(createMove(bitboard, noWe, false, true, true));
        else moves.insert(createMove(bitboard, noWe, false, true, false));
    if (soEa != 0)
        if (soEa & colorBitboard) moves.insert(createMove(bitboard, soEa, false, true, true));
        else moves.insert(createMove(bitboard, soEa, false, true, false));
    if (soWe != 0)
        if (soWe & colorBitboard) moves.insert(createMove(bitboard, soWe, false, true, true));
        else moves.insert(createMove(bitboard, soWe, false, true, false));

    return moves;
}

