#include <board.hpp>

std::unordered_set<Move, MoveHash> Board::generateKingMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t attackedSquare, Direction attackerDirection, Color color) {
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
        if (ea & colorBitboard) moves.insert(createMove(bitboard, ea, false, true, true, Ea));
        else if (attackerDirection != Ea) moves.insert(createMove(bitboard, ea, false, true, false, Ea));
    if (we != 0)
        if (we & colorBitboard) moves.insert(createMove(bitboard, we, false, true, true, We));
        else if (attackerDirection != We) moves.insert(createMove(bitboard, we, false, true, false, We));
    if (no != 0)
        if (no & colorBitboard) moves.insert(createMove(bitboard, no, false, true, true, No));
        else if (attackerDirection != No) moves.insert(createMove(bitboard, no, false, true, false, No));
    if (so != 0)
        if (so & colorBitboard) moves.insert(createMove(bitboard, so, false, true, true, So));
        else if (attackerDirection != So) moves.insert(createMove(bitboard, so, false, true, false, So));
    if (noEa != 0)
        if (noEa & colorBitboard) moves.insert(createMove(bitboard, noEa, false, true, true, NoEa));
        else if (attackerDirection != NoEa) moves.insert(createMove(bitboard, noEa, false, true, false, NoEa));
    if (noWe != 0)
        if (noWe & colorBitboard) moves.insert(createMove(bitboard, noWe, false, true, true, NoWe));
        else if (attackerDirection != NoWe) moves.insert(createMove(bitboard, noWe, false, true, false, NoWe));
    if (soEa != 0)
        if (soEa & colorBitboard) moves.insert(createMove(bitboard, soEa, false, true, true, SoEa));
        else if (attackerDirection != SoEa) moves.insert(createMove(bitboard, soEa, false, true, false, SoEa));
    if (soWe != 0)
        if (soWe & colorBitboard) moves.insert(createMove(bitboard, soWe, false, true, true, SoWe));
        else if (attackerDirection != SoWe) moves.insert(createMove(bitboard, soWe, false, true, false, SoWe));

    return moves;
}

