#include <board.hpp>

std::vector<Move> Board::generateKingMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t attackedSquare, Direction attackerDirection, Color color) {
    std::vector<Move> moves;
    uint64_t ea = eastOne(bitboard) & ~attackedSquare;
    uint64_t we = westOne(bitboard) & ~attackedSquare;
    uint64_t no = (bitboard << 8) & ~attackedSquare;
    uint64_t so = (bitboard >> 8) & ~attackedSquare;
    uint64_t noEa = noEaOne(bitboard) & ~attackedSquare;
    uint64_t noWe = noWeOne(bitboard) & ~attackedSquare;
    uint64_t soEa = soEaOne(bitboard) & ~attackedSquare;
    uint64_t soWe = soWeOne(bitboard) & ~attackedSquare;

    if (!inCheck) {
        uint64_t baseLineGlobal = globalBitboard >> ((color == White) ? 0 : 7*8);
        uint64_t baseLineAttacked = attackedSquare >> ((color == White) ? 0 : 7*8);

        if (castling[color][QueenSide] && !((uint64_t) 0b1110 & baseLineGlobal) && !((uint64_t) 0b1100 & baseLineAttacked))
            moves.push_back(createMove(bitboard, (uint64_t) 0b100 << ((color == White) ? 0: 7*8), false, false, false, true, false, We));
        
        if (castling[color][KingSide] && !((uint64_t) 0b1100000 & baseLineGlobal) && !((uint64_t) 0b1100000 & baseLineAttacked))
            moves.push_back(createMove(bitboard, (uint64_t) 0b1000000 << ((color == White) ? 0: 7*8), false, false, false, true, false, Ea));
    }

    if (ea != 0)
        if (ea & colorBitboard) moves.push_back(createMove(bitboard, ea, false, true, true, false, false, Ea));
        else if (attackerDirection != Ea) moves.push_back(createMove(bitboard, ea, false, true, false, false, false, Ea));
    if (we != 0)
        if (we & colorBitboard) moves.push_back(createMove(bitboard, we, false, true, true, false, false, We));
        else if (attackerDirection != We) moves.push_back(createMove(bitboard, we, false, true, false, false, false, We));
    if (no != 0)
        if (no & colorBitboard) moves.push_back(createMove(bitboard, no, false, true, true, false, false, No));
        else if (attackerDirection != No) moves.push_back(createMove(bitboard, no, false, true, false, false, false, No));
    if (so != 0)
        if (so & colorBitboard) moves.push_back(createMove(bitboard, so, false, true, true, false, false, So));
        else if (attackerDirection != So) moves.push_back(createMove(bitboard, so, false, true, false, false, false, So));
    if (noEa != 0)
        if (noEa & colorBitboard) moves.push_back(createMove(bitboard, noEa, false, true, true, false, false, NoEa));
        else if (attackerDirection != NoEa) moves.push_back(createMove(bitboard, noEa, false, true, false, false, false, NoEa));
    if (noWe != 0)
        if (noWe & colorBitboard) moves.push_back(createMove(bitboard, noWe, false, true, true, false, false, NoWe));
        else if (attackerDirection != NoWe) moves.push_back(createMove(bitboard, noWe, false, true, false, false, false, NoWe));
    if (soEa != 0)
        if (soEa & colorBitboard) moves.push_back(createMove(bitboard, soEa, false, true, true, false, false, SoEa));
        else if (attackerDirection != SoEa) moves.push_back(createMove(bitboard, soEa, false, true, false, false, false, SoEa));
    if (soWe != 0)
        if (soWe & colorBitboard) moves.push_back(createMove(bitboard, soWe, false, true, true, false, false, SoWe));
        else if (attackerDirection != SoWe) moves.push_back(createMove(bitboard, soWe, false, true, false, false, false, SoWe));

    return moves;
}

