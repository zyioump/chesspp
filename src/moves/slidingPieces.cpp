#include <board.hpp>

std::unordered_set<Move, MoveHash> Board::generateSlidingPiecesMoves(uint64_t bitboard, std::unordered_set<Direction> rayDirections, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color) {
    std::unordered_set<Move, MoveHash> moves;

    int square = log2(bitboard);

    uint64_t attacks = 0;

    for (Direction direction: rayDirections) {
        uint64_t ray = rayAttackBitboard[square][direction];
        uint64_t xrayAttackedPiece = ray & globalBitboard;
        if (xrayAttackedPiece == 0) {
            attacks |= ray;
            continue;
        }

        int ls1bXraySquare;
        if (direction == No || direction == Ea || direction == NoWe || direction == NoEa) ls1bXraySquare = forwardBitscan(xrayAttackedPiece);
        else ls1bXraySquare = reverseBitscan(xrayAttackedPiece);

        if (color != turn && piecesBitboards[turn][King] & ray) {
            int ls2bXraySquare;
            if (direction == No || direction == Ea || direction == NoWe || direction == NoEa) ls2bXraySquare = forwardBitscan(xrayAttackedPiece ^ (uint64_t) 1 << ls1bXraySquare);
            else ls2bXraySquare = reverseBitscan(xrayAttackedPiece ^ (uint64_t) 1 << ls1bXraySquare);

            if ((uint64_t) 1 << ls2bXraySquare == piecesBitboards[turn][King]) printf("%d pinned !\n", ls1bXraySquare);
        }

        uint64_t ls1bDirectionAttack = rayAttackBitboard[ls1bXraySquare][direction];

        attacks |= ray ^ ls1bDirectionAttack;
    }

    std::unordered_set<uint64_t> toSquares = serializeBitboard(attacks);
    for (uint64_t toSquare : toSquares)
        if (toSquare & colorBitboard) moves.insert(createMove(bitboard, toSquare, false, true, true));
        else moves.insert(createMove(bitboard, toSquare, false, true, false));

    return moves;
}
