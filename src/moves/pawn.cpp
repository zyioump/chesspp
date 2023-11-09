#include <board.hpp>

std::unordered_set<Move, MoveHash> Board::generatePawnMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color) {
    std::unordered_set<Move, MoveHash> moves;

    // Move 
    uint basePawnRow = (color == White) ? 1 : 6;
    uint64_t basePawnMask = 0b11111111;
    basePawnMask <<= 8*basePawnRow;

    uint maxMoveDistance = 1;
    uint64_t pawnOnBaseLine = bitboard & basePawnMask;
    if (pawnOnBaseLine != 0) maxMoveDistance++;

    uint promotionRow = (color == White) ? 7 : 0;
    uint64_t promotionMask = 0b11111111;
    promotionMask <<= 8*promotionRow;

    bool promotion = false;

    for (int moveDistance=1; moveDistance<=maxMoveDistance; moveDistance++) {
        promotion = false;

        uint64_t toBitboard;
        if (color == White) toBitboard = bitboard << 8*moveDistance;
        else toBitboard = bitboard >> 8*moveDistance;

        uint64_t isPromotion = promotionMask & toBitboard;
        if (isPromotion != 0) promotion = true;

        uint64_t toAnotherPiece = toBitboard & globalBitboard;
        if (toAnotherPiece != 0) break;

        moves.insert(createMove(bitboard, toBitboard, promotion, false, false));
    }

    uint64_t attackingRow = 0;
    if (color == White) attackingRow = bitboard << 8;
    else attackingRow = bitboard >> 8;

    uint64_t isPromotion = promotionMask & attackingRow;
    if (isPromotion != 0) promotion = true;

    // Attack
    if (color == White) {
        uint64_t noEaBitboard = noEaOne(bitboard);
        if (noEaBitboard & opponentColorBitboard) moves.insert(createMove(bitboard, noEaBitboard, promotion, true, false));
        if (noEaBitboard) moves.insert(createMove(bitboard, noEaBitboard, promotion, true, true));
        uint64_t noWeBitboard = noWeOne(bitboard);
        if (noWeBitboard & opponentColorBitboard) moves.insert(createMove(bitboard, noWeBitboard, promotion, true, false));
        if (noWeBitboard) moves.insert(createMove(bitboard, noWeBitboard, promotion, true, true));
    } else {
        uint64_t soEaBitboard = soEaOne(bitboard);
        if (soEaBitboard & opponentColorBitboard) moves.insert(createMove(bitboard, soEaBitboard, promotion, true, false));
        if (soEaBitboard) moves.insert(createMove(bitboard, soEaBitboard, promotion, true, true));
        uint64_t soWeBitboard = soWeOne(bitboard);
        if (soWeBitboard & opponentColorBitboard) moves.insert(createMove(bitboard, soWeBitboard, promotion, true, false));
        if (soWeBitboard) moves.insert(createMove(bitboard, soWeBitboard, promotion, true, true));
    }

    return moves;
}
