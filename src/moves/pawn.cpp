#include <board.hpp>

std::vector<Move> Board::generatePawnMoves(uint64_t bitboard, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color) {
    std::vector<Move> moves;

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
    Direction direction = (color == White) ? No : So;

    for (int moveDistance=1; moveDistance<=maxMoveDistance; moveDistance++) {
        promotion = false;

        uint64_t toBitboard;
        if (color == White) toBitboard = bitboard << 8*moveDistance;
        else toBitboard = bitboard >> 8*moveDistance;

        uint64_t isPromotion = promotionMask & toBitboard;
        if (isPromotion != 0) promotion = true;

        uint64_t toAnotherPiece = toBitboard & globalBitboard;
        if (toAnotherPiece != 0) break;

        moves.push_back(createMove(bitboard, toBitboard, promotion, false, false, false, false, direction));
    }

    uint64_t enPassantAttackerRowMask = (uint64_t) 0xFF << 8*((color == White) ? 4 : 3);
    if (enPassantCol >= 0 && bitboard & enPassantAttackerRowMask) {
        uint bitboardCol = reverseBitscan(bitboard) & 7;
        if (std::abs(int (bitboardCol - enPassantCol)) == 1) {
            uint64_t enPassantAttackBitboard = (uint64_t) 1 << (8*((color == White) ? 5 : 2) + enPassantCol);
            moves.push_back(createMove(bitboard, enPassantAttackBitboard, false, true, false, false, true, (color == White) ? No : So));
        }
    }

    uint64_t attackingRow = 0;
    if (color == White) attackingRow = bitboard << 8;
    else attackingRow = bitboard >> 8;

    uint64_t isPromotion = promotionMask & attackingRow;
    if (isPromotion != 0) promotion = true;

    // Attack
    if (color == White) {
        uint64_t noEaBitboard = noEaOne(bitboard);
        if (noEaBitboard & opponentColorBitboard) moves.push_back(createMove(bitboard, noEaBitboard, promotion, true, false, false, false, NoEa));
        if (noEaBitboard) moves.push_back(createMove(bitboard, noEaBitboard, promotion, true, true, false, false, NoEa));
        uint64_t noWeBitboard = noWeOne(bitboard);
        if (noWeBitboard & opponentColorBitboard) moves.push_back(createMove(bitboard, noWeBitboard, promotion, true, false, false, false, NoWe));
        if (noWeBitboard) moves.push_back(createMove(bitboard, noWeBitboard, promotion, true, true, false, false, NoWe));
    } else {
        uint64_t soEaBitboard = soEaOne(bitboard);
        if (soEaBitboard & opponentColorBitboard) moves.push_back(createMove(bitboard, soEaBitboard, promotion, true, false, false, false, SoEa));
        if (soEaBitboard) moves.push_back(createMove(bitboard, soEaBitboard, promotion, true, true, false, false, SoEa));
        uint64_t soWeBitboard = soWeOne(bitboard);
        if (soWeBitboard & opponentColorBitboard) moves.push_back(createMove(bitboard, soWeBitboard, promotion, true, false, false, false, SoWe));
        if (soWeBitboard) moves.push_back(createMove(bitboard, soWeBitboard, promotion, true, true, false, false, SoWe));
    }

    return moves;
}
