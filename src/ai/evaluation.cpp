#include "ai.hpp"

std::pair<int, int> Ai::evaluate(Board board) {
    int isBlack = (board.turn == White) ? 1 : -1;

    if (board.legalMoves.size() == 0) {
        if (board.inCheck) return std::make_pair(isBlack * (std::numeric_limits<int>::min() + 5), 0);
        else return std::make_pair(0, 0);
    }

    if (board.isRepetition()) return std::make_pair(0, 0);

    int earlyScore[2]  = {0};
    int endScore[2] = {0};
    int endGame=0;
    int score = 0;

    for (int pieceType=0; pieceType<6; pieceType++) {
        std::vector<int> pieces[2];
        for (int color=0; color<2; color++) {
            pieces[color] = board.pieces((PieceType) pieceType, (Color) color);
            PieceBonus pieceBonus = getPieceBonus((PieceType) pieceType, (Color) color, pieces[color]);

            earlyScore[color] += pieceBonus.earlyBonus;
            endScore[color] += pieceBonus.endBonus;
            endGame += pieceBonus.endGame;
        }
        /* score += getPieceValue((PieceType) pieceType) * (pieces[White].size() - pieces[Black].size()); */
    }

    int early = earlyScore[White] - earlyScore[Black];
    int end = endScore[White] - endScore[Black];
    if (endGame > 24) endGame = 24;

    score += (endGame * early + (24 - endGame) * end) / 24;

    auto currentTime = std::chrono::high_resolution_clock::now();
    if ((currentTime - moveStartTime).count() * 1e-9 >= timeLimit) stopSearching = true;

    return std::make_pair(isBlack * score, (float) endGame);
}

int Ai::transposeSquareForBonus(int square, Color color) {
    int col = square & 7;
    int row = (square - col)/8;
    if (color == Black) row = 7 - row;

    int index = (7 - row) * 8 + col;

    return index;
}

PieceBonus Ai::getPieceBonus(PieceType pieceType, Color color, std::vector<int> pieceSquares) {
    PieceBonus pieceBonus;
    pieceBonus.earlyBonus = 0;
    pieceBonus.endBonus = 0;
    pieceBonus.endGame = 0;

    int earlyPieceValue = 0;
    int endPieceValue = 0;
    const int* ealyBonusArray;
    const int* endBonusArray;

    switch (pieceType) {
        case King: 
            ealyBonusArray = kingSquareEarlyBonus; 
            endBonusArray = kingSquareEndBonus; 
            break;
        case Queen:
            ealyBonusArray = queenSquareEarlyBonus; 
            endBonusArray = queenSquareEndBonus; 
            pieceBonus.endGame = 4;
            earlyPieceValue = 1025;
            endPieceValue = 936;
            break;
        case Rook:
            ealyBonusArray = rookSquareEarlyBonus; 
            endBonusArray = rookSquareEndBonus; 
            pieceBonus.endGame = 2;
            earlyPieceValue = 477;
            endPieceValue = 512;
            break;
        case Bishop:
            ealyBonusArray = bishopSquareEarlyBonus; 
            endBonusArray = bishopSquareEndBonus; 
            pieceBonus.endGame = 1;
            earlyPieceValue = 366;
            endPieceValue = 297;
            break;
        case Knight:
            ealyBonusArray = knightSquareEarlyBonus; 
            endBonusArray = knightSquareEndBonus; 
            pieceBonus.endGame = 1;
            earlyPieceValue = 337;
            endPieceValue = 281;
            break;
        case Pawn:
            ealyBonusArray = pawnSquareEarlyBonus; 
            endBonusArray = pawnSquareEndBonus; 
            earlyPieceValue = 82;
            endPieceValue = 94;
            break;
    }

    int pieceNum = 0;
    for (int square: pieceSquares) {
        int bonusIndex = transposeSquareForBonus(square, color);
        pieceBonus.earlyBonus += ealyBonusArray[bonusIndex] + earlyPieceValue;
        pieceBonus.endBonus += endBonusArray[bonusIndex] + endPieceValue;
        pieceNum++;
    }
    pieceBonus.endGame *= pieceNum;
    return pieceBonus;
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
