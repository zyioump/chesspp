#include <board.hpp>


std::unordered_set<Move, MoveHash> Board::generateKnightMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t oppentColorBitboard, Color color) {
    std::unordered_set<Move, MoveHash> moves;

    uint64_t noNoEa = (bitboard & noHCol) << 17;
    uint64_t noEaEa = (bitboard & noGHCol) << 10;
    uint64_t soEaEa = (bitboard & noGHCol) >> 6;
    uint64_t soSoEa = (bitboard & noHCol) >> 15;
    uint64_t noNoWe = (bitboard & noACol) << 15;
    uint64_t noWeWe = (bitboard & noABCol) << 6;
    uint64_t soWeWe = (bitboard & noABCol) >> 10;
    uint64_t soSoWe = (bitboard & noACol) >> 17;

    if (noNoEa != 0)
        if (noNoEa & colorBitboard) moves.insert(createMove(bitboard, noNoEa, false, true, true));
        else moves.insert(createMove(bitboard, noNoEa, false, true, false));
    if (noEaEa != 0)
        if (noEaEa & colorBitboard) moves.insert(createMove(bitboard, noEaEa, false, true, true));
        else moves.insert(createMove(bitboard, noEaEa, false, true, false));
    if (soEaEa != 0)
        if (soEaEa & colorBitboard) moves.insert(createMove(bitboard, soEaEa, false, true, true));
        else moves.insert(createMove(bitboard, soEaEa, false, true, false));
    if (soSoEa != 0)
        if (soSoEa & colorBitboard) moves.insert(createMove(bitboard, soSoEa, false, true, true));
        else moves.insert(createMove(bitboard, soSoEa, false, true, false));
    if (noNoWe != 0)
        if (noNoWe & colorBitboard) moves.insert(createMove(bitboard, noNoWe, false, true, true));
        else moves.insert(createMove(bitboard, noNoWe, false, true, false));
    if (noWeWe != 0)
        if (noWeWe & colorBitboard) moves.insert(createMove(bitboard, noWeWe, false, true, true));
        else moves.insert(createMove(bitboard, noWeWe, false, true, false));
    if (soWeWe != 0)
        if (soWeWe & colorBitboard) moves.insert(createMove(bitboard, soWeWe, false, true, true));
        else moves.insert(createMove(bitboard, soWeWe, false, true, false));
    if (soSoWe != 0)
        if (soSoWe & colorBitboard) moves.insert(createMove(bitboard, soSoWe, false, true, true));
        else moves.insert(createMove(bitboard, soSoWe, false, true, false));

    return moves;
} 
