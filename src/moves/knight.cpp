#include <board.hpp>


std::vector<Move> Board::generateKnightMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t oppentColorBitboard, Color color) {
    std::vector<Move> moves;

    uint64_t noNoEa = (bitboard & noHCol) << 17;
    uint64_t noEaEa = (bitboard & noGHCol) << 10;
    uint64_t soEaEa = (bitboard & noGHCol) >> 6;
    uint64_t soSoEa = (bitboard & noHCol) >> 15;
    uint64_t noNoWe = (bitboard & noACol) << 15;
    uint64_t noWeWe = (bitboard & noABCol) << 6;
    uint64_t soWeWe = (bitboard & noABCol) >> 10;
    uint64_t soSoWe = (bitboard & noACol) >> 17;

    if (noNoEa != 0)
        if (noNoEa & colorBitboard) moves.push_back(createMove(bitboard, noNoEa, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, noNoEa, false, true, false, false, false, KnightDir));
    if (noEaEa != 0)
        if (noEaEa & colorBitboard) moves.push_back(createMove(bitboard, noEaEa, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, noEaEa, false, true, false, false, false, KnightDir));
    if (soEaEa != 0)
        if (soEaEa & colorBitboard) moves.push_back(createMove(bitboard, soEaEa, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, soEaEa, false, true, false, false, false, KnightDir));
    if (soSoEa != 0)
        if (soSoEa & colorBitboard) moves.push_back(createMove(bitboard, soSoEa, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, soSoEa, false, true, false, false, false, KnightDir));
    if (noNoWe != 0)
        if (noNoWe & colorBitboard) moves.push_back(createMove(bitboard, noNoWe, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, noNoWe, false, true, false, false, false, KnightDir));
    if (noWeWe != 0)
        if (noWeWe & colorBitboard) moves.push_back(createMove(bitboard, noWeWe, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, noWeWe, false, true, false, false, false, KnightDir));
    if (soWeWe != 0)
        if (soWeWe & colorBitboard) moves.push_back(createMove(bitboard, soWeWe, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, soWeWe, false, true, false, false, false, KnightDir));
    if (soSoWe != 0)
        if (soSoWe & colorBitboard) moves.push_back(createMove(bitboard, soSoWe, false, true, true, false, false, KnightDir));
        else moves.push_back(createMove(bitboard, soSoWe, false, true, false, false, false, KnightDir));

    return moves;
} 
