#include "board.hpp"
#include "utils.hpp"

EnemyAttack Board::getEnemyAttackBitboard(uint64_t occupancy) {
    uint64_t attackBitboard = 0;
    uint64_t captureProtect = 0xffffffffffffffff;
    uint64_t moveProtect = 0xffffffffffffffff;
    int checkNum = 0;

    int kingSquare = reverseBitscan(piecesBitboards[turn][King]);
    uint64_t kingBishopAttack = getSlidingPieceLegalAttack(kingSquare, occupancy, true);
    uint64_t kingRookAttack = getSlidingPieceLegalAttack(kingSquare, occupancy, false);

    std::vector<PinnedPiece> pinnedPieces;

    for (int pieceType=0; pieceType<6; pieceType++) {
        uint64_t pieceBitboard = piecesBitboards[!turn][pieceType];
        int square;
        if (pieceBitboard) do {
            square = reverseBitscan(pieceBitboard);
            uint64_t pieceAttack = 0;
            switch (pieceType) {
                case Pawn: pieceAttack = pawnAttack[!turn][square]; break;
                case Knight: pieceAttack = knightAttack[square]; break;
                case King: pieceAttack = kingAttack[square]; break;
                case Bishop:
                    pieceAttack = getSlidingPieceLegalAttack(square, occupancy, true);
                    findPinnedMask(square, pieceAttack, kingBishopAttack, occupancy, true, &pinnedPieces);
                    break;
                case Rook:
                    pieceAttack = getSlidingPieceLegalAttack(square, occupancy, false);
                    findPinnedMask(square, pieceAttack, kingRookAttack, occupancy, false, &pinnedPieces);
                    break;
                case Queen:
                    pieceAttack = getSlidingPieceLegalAttack(square, occupancy, false) | getSlidingPieceLegalAttack(square, occupancy, true);
                    findPinnedMask(square, pieceAttack, kingBishopAttack, occupancy, false, &pinnedPieces);
                    findPinnedMask(square, pieceAttack, kingRookAttack, occupancy, true, &pinnedPieces);
                    break;
            }

            if (piecesBitboards[turn][King] & pieceAttack) {
                checkNum++;
                moveProtect = 0;
                if (pieceType == Queen || pieceType == Rook || pieceType == Bishop)
                    moveProtect = pieceAttack;
                captureProtect = powOf2[square];
            }
            attackBitboard |= pieceAttack;
        } while (pieceBitboard ^= powOf2[square]);
    }

    EnemyAttack enemyAttack;
    enemyAttack.attack = attackBitboard;
    enemyAttack.checkNum = checkNum;
    enemyAttack.captureProtect = captureProtect;
    enemyAttack.moveProtect = moveProtect;
    enemyAttack.pinnedPieces = pinnedPieces;
    return enemyAttack;
}

void Board::findPinnedMask(int square, uint64_t pieceAttack, uint64_t kingPieceAttack, uint64_t occupancy, bool isBishop, std::vector<PinnedPiece>* pinnedPieces) {
    uint64_t intersection = pieceAttack & kingPieceAttack;
    if (count1(intersection) != 1) return;

    int pinnedSquare = reverseBitscan(intersection);
    uint64_t pinnedPieceAttack = getSlidingPieceLegalAttack(pinnedSquare, occupancy, isBishop);
    uint64_t pinnedMask = pinnedPieceAttack & (pieceAttack | kingPieceAttack);

    PinnedPiece pinnedPiece;
    pinnedPiece.mask = pinnedMask;
    pinnedPiece.attacker = powOf2[square];
    pinnedPiece.square = pinnedSquare;
    pinnedPieces->push_back(pinnedPiece);
    return;
}

void Board::generateMoves() {
    legalMoves.clear();

    uint64_t friendlyPieces = getColorBitboard(turn);
    uint64_t enemyPieces = getColorBitboard((Color) !turn);
    uint64_t occupancy = friendlyPieces | enemyPieces;
    EnemyAttack enemyAttack = getEnemyAttackBitboard(occupancy ^ piecesBitboards[turn][King]);

    if (enemyAttack.checkNum > 1) {
        int square = reverseBitscan(piecesBitboards[turn][King]);
        uint64_t kingMoves = kingAttack[square] & ~enemyAttack.attack & ~friendlyPieces;
        while (kingMoves) {
            uint64_t to = kingMoves & -kingMoves;
            legalMoves.push_back(createMove(piecesBitboards[turn][King], to, false, false, false));
            kingMoves &= kingMoves-1;
        }
        return;
    }

    for (int pieceType=0; pieceType<6; pieceType++) {
        uint64_t pieceBitboard = piecesBitboards[turn][pieceType];
        int square;
        if (pieceBitboard) do {
            square = reverseBitscan(pieceBitboard);
            uint64_t moveBitboard = 0;
            switch (pieceType) {
                case Pawn:
                    moveBitboard = pawnAttack[turn][square] & enemyPieces;

                    if ((powOf2[square + ((turn == White) ? 8 : -8)] & occupancy) == 0)
                        moveBitboard |= pawnMove[turn][square] & ~occupancy;

                    if (enPassantCol > 0) {
                        int distanceToEnPassant = abs(enPassantCol - ((square) & 7));
                        bool isOnEnPassantRank = (square >> 3) == ((turn == White) ? 4 : 3);
                        if (distanceToEnPassant == 1 && isOnEnPassantRank) {
                            int toSquare = enPassantCol + 8*((turn == White) ? 5 : 2);
                            uint64_t to = powOf2[toSquare];

                            int kingSquare = reverseBitscan(piecesBitboards[turn][King]);
                            if ((kingSquare >> 3) == ((turn == White) ? 4:3)) {
                                uint64_t occupancyWithoutEnPassant = occupancy ^ powOf2[square] ^ powOf2[enPassantCol + 8*((turn == White) ? 4 : 3)];
                                uint64_t kingRookAttack = getSlidingPieceLegalAttack(kingSquare, occupancyWithoutEnPassant, false);
                                if ((kingRookAttack & piecesBitboards[!turn][Queen]) || (kingRookAttack & piecesBitboards[!turn][Rook])) to = 0;
                            }

                            if (to & enemyAttack.moveProtect) legalMoves.push_back(createMove(powOf2[square], to, false, false, true));
                            else if (((turn == White) ? to >> 8 : to << 8) & enemyAttack.captureProtect) legalMoves.push_back(createMove(powOf2[square], to, false, false, true));
                        }
                    }
                    break;
                case Knight: moveBitboard = knightAttack[square]; break;
                case King:
                    moveBitboard = kingAttack[square] & ~enemyAttack.attack;
                    if (enemyAttack.checkNum == 0) {
                        uint64_t kingBitboard = piecesBitboards[turn][King];
                        if (castling[turn][QueenSide] && !(kingBitboard >> 1 & occupancy) && !(kingBitboard >> 2 & occupancy) && !(kingBitboard >> 3 & occupancy))
                            legalMoves.push_back(createMove(kingBitboard, kingBitboard >> 2, false, true, false));
                        if (castling[turn][KingSide] && !(kingBitboard << 1 & occupancy) && !(kingBitboard << 2 & occupancy))
                            legalMoves.push_back(createMove(kingBitboard, kingBitboard << 2, false, true, false));
                    }
                    break;
                case Bishop: moveBitboard = getSlidingPieceLegalAttack(square, occupancy, true); break;
                case Rook: moveBitboard = getSlidingPieceLegalAttack(square, occupancy, false); break;
                case Queen: moveBitboard = getSlidingPieceLegalAttack(square, occupancy, false) | getSlidingPieceLegalAttack(square, occupancy, true); break;
            }

            moveBitboard &= ~friendlyPieces;
            if (pieceType != King) {
                moveBitboard &= (enemyAttack.captureProtect | enemyAttack.moveProtect);

                if (enemyAttack.pinnedPieces.size() > 0) {
                    auto pinnedPiece = std::find(enemyAttack.pinnedPieces.begin(), enemyAttack.pinnedPieces.end(), square);
                    if (pinnedPiece != enemyAttack.pinnedPieces.end()) {
                        moveBitboard &= pinnedPiece->mask | pinnedPiece->attacker;
                    }
                }
            }

            while (moveBitboard) {
                uint64_t to = moveBitboard & -moveBitboard;
                if (pieceType == Pawn && (to & ~no1Row & ~no8Row)) legalMoves.push_back(createMove(powOf2[square], to, true, false, false));
                else legalMoves.push_back(createMove(powOf2[square], to, false, false, false));
                moveBitboard &= moveBitboard-1;
            }
        } while (pieceBitboard ^= powOf2[square]);
    }
}
