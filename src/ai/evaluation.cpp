#include "ai.hpp"

int Ai::evaluate(Board board) {
    int score = 0;
    int isBlack = (board.turn == White) ? 1 : -1;

    for (int pieceType=0; pieceType<6; pieceType++) {
        int pieceValue = getPieceValue((PieceType) pieceType);

        auto whitePieces = board.pieces((PieceType) pieceType, White);
        auto blackPieces = board.pieces((PieceType) pieceType, Black);

        score += pieceValue * (whitePieces.size() - blackPieces.size());
        score += getPiecesBonus((PieceType) pieceType, White, whitePieces) - getPiecesBonus((PieceType) pieceType, Black, blackPieces);
    }

    return isBlack * score;
}

int Ai::transposeSquareForBonus(int square, Color color) {
    int col = square & 7;
    int row = (square - col)/8;
    if (color == Black) row = 7 - row;

    int index = (7 - row) * 8 + col;

    return index;
}

int Ai::getPiecesBonus(PieceType pieceType, Color color, std::unordered_set<uint64_t> bitboards) {
    int bonus = 0;
    for (uint64_t bitboard: bitboards) {
        int square = log2(bitboard);
        int bonusIndex = transposeSquareForBonus(square, color);
        switch (pieceType) {
            case King: bonus += kingSquareBonus[bonusIndex]; break;
            case Queen: bonus += queenSquareBonus[bonusIndex]; break;
            case Rook: bonus += rookSquareBonus[bonusIndex]; break;
            case Bishop: bonus += bishopSquareBonus[bonusIndex]; break;
            case Knight: bonus += knightSquareBonus[bonusIndex]; break;
            case Pawn: bonus += pawnSquareBonus[bonusIndex]; break;
        }
    }
    return bonus;
}

int Ai::getPieceValue(PieceType pieceType) {
    switch ((PieceType) pieceType) {
        case King: return 20000;
        case Queen: return 900;
        case Rook: return 500;
        case Bishop: return 330;
        case Knight: return 320;
        case Pawn: return 100;
    }
    return 0;
}
