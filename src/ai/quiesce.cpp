#include <ai.hpp>

using std::chrono::high_resolution_clock;

int Ai::see(Board board, uint64_t bitboard, int pieceValue) {
    Move minAttackerMove;
    int minAttackerValue = std::numeric_limits<int>::max();

    Piece attacker;
    int value;
    for (Move move: board.legalMoves){
        if (move.to != bitboard) continue;

        board.pieceAtBitboard(move.from, &attacker);
        
        value = getPieceValue(attacker.pieceType);
        if (value < minAttackerValue) {
            minAttackerValue = value;
            minAttackerMove = move;
        }
    }

    if (minAttackerValue == std::numeric_limits<int>::max()) return 0;

    auto start = high_resolution_clock::now();
    board.push(minAttackerMove);
    auto stop = high_resolution_clock::now();
    metrics["Push time"] += (stop - start).count() * 1e-9;

    int score = pieceValue - see(board, bitboard, minAttackerValue);

    start = high_resolution_clock::now();
    board.pop();
    stop = high_resolution_clock::now();
    metrics["Pop time"] += (stop - start).count() * 1e-9;

    return score;
}

int Ai::seeCapture(Board board, Move move) {
    Piece toPiece;
    board.pieceAtBitboard(move.to, &toPiece);
    int toPieceValue = getPieceValue(toPiece.pieceType);

    Piece fromPiece;
    board.pieceAtBitboard(move.from, &fromPiece);
    int fromPieceValue = getPieceValue(fromPiece.pieceType);

    auto start = high_resolution_clock::now();
    board.push(move);
    auto stop = high_resolution_clock::now();
    metrics["Push time"] += (stop - start).count() * 1e-9;

    int score = toPieceValue - see(board, move.to, fromPieceValue);

    start = high_resolution_clock::now();
    board.pop();
    stop = high_resolution_clock::now();
    metrics["Pop time"] += (stop - start).count() * 1e-9;

    return score;
}

int Ai::quiesce(Board board, int alpha, int beta) {
    metrics["Evaluation num"]++;
    auto start = high_resolution_clock::now();
    auto evaluation = evaluate(board);
    int currentScore = evaluation.first;
    int endGame = evaluation.second;
    auto stop = high_resolution_clock::now();
    metrics["Evaluation time"] += (stop - start).count() * 1e-9;

    /* return currentScore; */

    if (currentScore >= beta) return beta;
    if (alpha < currentScore) alpha = currentScore;

    std::vector<std::pair<Move, int>> legalMoves = orderMove(board, board.legalMoves, nullptr, nullptr);

    Move move;
    Piece attackedPiece;
    int seeScore;
    int score;
    for (auto moveScore: legalMoves) {
        move = moveScore.first;

        if (!board.pieceAtBitboard(move.to, &attackedPiece)) continue;
        /* std::cout << "Quiesce " << bitboardToSquareName(move.from) << " to " <<  bitboardToSquareName(move.to) << ", depth " << depth << "\n"; */

        if (endGame > 6) {
            start = high_resolution_clock::now();
            seeScore = seeCapture(board, move);
            stop = high_resolution_clock::now();
            metrics["See time"] += (stop - start).count() * 1e-9;

            if (seeScore < 0) {
                metrics["See skip"]++;
                continue;
            }
        }

        auto start = high_resolution_clock::now();
        board.push(move);
        auto stop = high_resolution_clock::now();
        metrics["Push time"] += (stop - start).count() * 1e-9;

        score = -quiesce(board, -beta, -alpha);

        start = high_resolution_clock::now();
        board.pop();
        stop = high_resolution_clock::now();
        metrics["Pop time"] += (stop - start).count() * 1e-9;

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}
