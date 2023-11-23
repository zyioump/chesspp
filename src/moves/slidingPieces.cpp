#include <board.hpp>

std::vector<Move> Board::generateSlidingPiecesMoves(uint64_t bitboard, std::vector<Direction> rayDirections, uint64_t globalBitboard, uint64_t colorBitboard, uint64_t opponentColorBitboard, Color color) {
    std::vector<Move> moves;

    int square = reverseBitscan(bitboard);

    for (Direction direction: rayDirections) {
        uint64_t ray = rayAttackBitboard[square][direction];
        uint64_t xrayAttackedPiece = ray & globalBitboard;
        uint64_t attacks = 0;

        if (xrayAttackedPiece == 0) {
            attacks = ray;
        } else {
            int ls1bXraySquare;
            if (direction == No || direction == Ea || direction == NoWe || direction == NoEa) ls1bXraySquare = forwardBitscan(xrayAttackedPiece);
            else ls1bXraySquare = reverseBitscan(xrayAttackedPiece);

            if (ls1bXraySquare == reverseBitscan(piecesBitboards[turn][King])) {
                PinnedPiece pinnedPiece;
                pinnedPiece.square = ls1bXraySquare;
                pinnedPiece.direction = direction;
                pinnedPieces.push_back(pinnedPiece);
            } else if (color != turn && piecesBitboards[turn][King] & ray) {
                int ls2bXraySquare;
                if (direction == No || direction == Ea || direction == NoWe || direction == NoEa) ls2bXraySquare = forwardBitscan(xrayAttackedPiece ^ (uint64_t) 1 << ls1bXraySquare);
                else ls2bXraySquare = reverseBitscan(xrayAttackedPiece ^ (uint64_t) 1 << ls1bXraySquare);

                if ((uint64_t) 1 << ls2bXraySquare == piecesBitboards[turn][King]) {
                    PinnedPiece pinnedPiece;
                    pinnedPiece.square = ls1bXraySquare;
                    pinnedPiece.direction = direction;
                    pinnedPieces.push_back(pinnedPiece);
                }
            }

            uint64_t ls1bDirectionAttack = rayAttackBitboard[ls1bXraySquare][direction];

            attacks = ray ^ ls1bDirectionAttack;
        }

        std::vector<uint64_t> toSquares = serializeBitboard(attacks);
        for (uint64_t toSquare : toSquares)
            if (toSquare & colorBitboard) moves.push_back(createMove(bitboard, toSquare, false, true, true, false, false, direction));
            else moves.push_back(createMove(bitboard, toSquare, false, true, false, false, false, direction));
    }

    return moves;
}
