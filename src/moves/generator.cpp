#include "board.hpp"

std::vector<Move> Board::generateColorMoves(uint64_t colorBitboard, uint64_t opponentColorBitboard, uint64_t globalBitboard, Color color, std::vector<Move>* opponentLegalMovesPtr) {
    inCheck = false;
    uint kingAttackers = 0;
    uint64_t kingAttackerBitboard;
    Direction kingAttackerDirection = NoDirection;

    uint64_t attackedSquare = 0;

    if (opponentLegalMovesPtr != nullptr) 
        for (Move opponentMove: *opponentLegalMovesPtr)
            if (opponentMove.attack) {
                attackedSquare |= opponentMove.to;
                if (piecesBitboards[color][King] == opponentMove.to) {
                    kingAttackers++;
                    kingAttackerBitboard = opponentMove.from;

                    Piece kingAttacker;
                    pieceAt(kingAttackerBitboard, &kingAttacker);
                    if (kingAttacker.pieceType == Queen || kingAttacker.pieceType == Rook || kingAttacker.pieceType == Bishop)
                        kingAttackerDirection = opponentMove.direction;
                }
            }

    uint64_t kingProtectionSquare = 0xffffffffffffffff;
    if (kingAttackers > 0) {
        inCheck = true;
        if (kingAttackers == 1) {
            kingProtectionSquare = kingAttackerBitboard;
            for (Move opponentMove: *opponentLegalMovesPtr)
                if (opponentMove.attack && opponentMove.from == kingAttackerBitboard && opponentMove.direction == kingAttackerDirection) 
                    kingProtectionSquare |= opponentMove.to;
        }
    }

    std::vector<Move> moves;

    if (kingAttackers <=1) {
        for (int pieceType=0; pieceType<6; pieceType++){
            for (uint64_t pieceBitboard : pieces((PieceType) pieceType, color)){
                std::vector<Move> piece_moves;
                switch (pieceType) {
                    case Pawn: piece_moves = generatePawnMoves(pieceBitboard, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case Knight: piece_moves = generateKnightMoves(pieceBitboard, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case King: piece_moves = generateKingMoves(pieceBitboard, globalBitboard, colorBitboard, opponentColorBitboard, attackedSquare, kingAttackerDirection, color); break;
                    case Queen: piece_moves = generateSlidingPiecesMoves(pieceBitboard, queenDirections, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case Rook: piece_moves = generateSlidingPiecesMoves(pieceBitboard, rookDirections, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                    case Bishop: piece_moves = generateSlidingPiecesMoves(pieceBitboard, bishopDirections, globalBitboard, colorBitboard, opponentColorBitboard, color); break;
                }
                if (kingAttackers == 1 && pieceType != King) piece_moves = removeNonKingProtectionMove(piece_moves, kingProtectionSquare);
                std::vector<Move> legalMove = removeNonLegalMove(piece_moves, pieceBitboard, (PieceType) pieceType, kingAttackerBitboard);
                moves.insert(moves.begin(), legalMove.begin(), legalMove.end());
            }
        }
    } else {
        return generateKingMoves(piecesBitboards[color][King], globalBitboard, colorBitboard, opponentColorBitboard, attackedSquare, kingAttackerDirection, color);
    }
    return moves;
}

 std::vector<Move> Board::removeNonKingProtectionMove(std::vector<Move> moves, uint64_t kingProtectionSquare) {
     std::vector<Move> legalMoves;
     for (Move move: moves) {
         if (move.to & kingProtectionSquare) legalMoves.push_back(move);
     }
     return legalMoves;
 }

 std::vector<Move> Board::removeNonLegalMove(std::vector<Move> moves, uint64_t pieceBitboard, PieceType pieceType, uint64_t kingAttackerBitboard) {
     PinnedPiece searchPinnedPiece = PinnedPiece();
     searchPinnedPiece.square = reverseBitscan(pieceBitboard);

     std::vector<PinnedPiece>::iterator pinnedPieceItr = std::find(pinnedPieces.begin(), pinnedPieces.end(), searchPinnedPiece);
     if (pinnedPieceItr == pinnedPieces.end()) return moves;

     std::vector<Move> pieceLegalMoves;
     if (pieceType == Knight) return pieceLegalMoves;

     Direction pinnedDirection = pinnedPieceItr->direction;
     Direction complementaryPinnedDirection = (pinnedDirection >= 4) ? Direction (pinnedDirection - 4) : Direction (pinnedDirection + 4);

     if (pieceType == King) {
         for (Move move: moves) {
             if (move.to == kingAttackerBitboard) pieceLegalMoves.push_back(move);
             else if (move.direction != pinnedDirection && move.direction != complementaryPinnedDirection) pieceLegalMoves.push_back(move);
         }
     } else {
         for (Move move: moves)
             if (move.direction == pinnedDirection || move.direction == complementaryPinnedDirection) pieceLegalMoves.push_back(move);
     }

     return pieceLegalMoves;
}

void Board::generateMoves(){
    uint64_t colorBitboard = getColorBitboard(turn);
    uint64_t opponentColorBitboard = getColorBitboard((Color) !turn);
    uint64_t globalBitboard = opponentColorBitboard | colorBitboard;

    pinnedPieces.clear();

    opponentLegalMoves = generateColorMoves(opponentColorBitboard, colorBitboard, globalBitboard, (Color) !turn, nullptr);
    legalMoves = generateColorMoves(colorBitboard, opponentColorBitboard, globalBitboard, turn, &opponentLegalMoves);
}
