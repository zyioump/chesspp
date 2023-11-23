#include "ai.hpp"

std::pair<int, bool> Ai::evaluate(Board board) {
    int score = 0;
    bool endGame = false;
    int isBlack = (board.turn == White) ? 1 : -1;

    int whiteQueenNum = board.pieces(Queen, White).size();
    int blackQueenNum = board.pieces(Queen, Black).size();
    int whiteRookNum = board.pieces(Rook, White).size();
    int blackRookNum = board.pieces(Rook, Black).size();

    if (whiteRookNum == 0 && blackQueenNum == 0 && whiteRookNum <= 1 && blackRookNum <= 1) endGame = true;
    else if (whiteQueenNum + blackQueenNum == 1) {
        Color noQueenSideColor = (whiteQueenNum == 1) ? Black : White;
        int noQueenSideRookNum = (noQueenSideColor == White) ? whiteRookNum : blackRookNum;
        if (noQueenSideRookNum == 0 && (board.pieces(Bishop, noQueenSideColor).size() + board.pieces(Bishop, noQueenSideColor).size()) <= 1)
            endGame = true;
    }

    for (int pieceType=0; pieceType<6; pieceType++) {
        int pieceValue = getPieceValue((PieceType) pieceType);

        auto whitePieces = board.pieces((PieceType) pieceType, White);
        auto blackPieces = board.pieces((PieceType) pieceType, Black);

        score += pieceValue * (whitePieces.size() - blackPieces.size());
        score += getPiecesBonus((PieceType) pieceType, White, whitePieces, endGame) - getPiecesBonus((PieceType) pieceType, Black, blackPieces, endGame);
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    if ((currentTime - moveStartTime).count() * 1e-9 >= timeLimit) stopSearching = true;

    return std::make_pair(isBlack * score, endGame);
}

int Ai::transposeSquareForBonus(int square, Color color) {
    int col = square & 7;
    int row = (square - col)/8;
    if (color == Black) row = 7 - row;

    int index = (7 - row) * 8 + col;

    return index;
}

int Ai::getPiecesBonus(PieceType pieceType, Color color, std::vector<uint64_t> bitboards, bool endGame) {
    int bonus = 0;
    int* bonusArray;

    switch (pieceType) {
        case King: bonusArray = endGame ? kingEndGameBonus : kingSquareBonus; break;
        case Queen: bonusArray = queenSquareBonus; break;
        case Rook: bonusArray = rookSquareBonus; break;
        case Bishop: bonusArray = bishopSquareBonus; break;
        case Knight: bonusArray = knightSquareBonus; break;
        case Pawn: bonusArray = pawnSquareBonus; break;
    }
    for (uint64_t bitboard: bitboards) {
        int square = reverseBitscan(bitboard);
        int bonusIndex = transposeSquareForBonus(square, color);
        bonus += bonusArray[bonusIndex];
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
