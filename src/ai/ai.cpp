#include <ai.hpp>

using std::chrono::high_resolution_clock;

int Ai::see(Board board, uint64_t bitboard, int pieceValue) {
    Move minAttackerMove;
    int minAttackerValue = std::numeric_limits<int>::max();

    for (Move move: board.legalMoves){
        if (!move.attack || move.defend) continue;
        if (move.to != bitboard) continue;

        Piece attacker;
        board.pieceAtBitboard(move.from, &attacker);
        
        int value = getPieceValue(attacker.pieceType);
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

int Ai::quiesce(Board board, int alpha, int beta, int depth) {
    metrics["Evaluation num"]++;
    auto start = high_resolution_clock::now();
    auto evaluation = evaluate(board);
    int currentScore = evaluation.first;
    bool endGame = evaluation.second;
    auto stop = high_resolution_clock::now();
    metrics["Evaluation time"] += (stop - start).count() * 1e-9;
    /* return currentScore; */

    if (depth == 0) return currentScore;

    if (currentScore >= beta) return beta;
    if (alpha < currentScore) alpha = currentScore;

    std::vector<std::pair<Move, int>> legalMoves = orderMove(board, board.legalMoves);

    for (auto moveScore: legalMoves) {
        Move move = moveScore.first;
        if (move.defend || !move.attack) continue;

        Piece attackedPiece;
        if (!board.pieceAtBitboard(move.to, &attackedPiece)) continue;
        /* std::cout << "Quiesce " << bitboardToSquareName(move.from) << " to " <<  bitboardToSquareName(move.to) << ", depth " << depth << "\n"; */

        if (!endGame) {
            start = high_resolution_clock::now();
            int seeScore = seeCapture(board, move);
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

        int score = -quiesce(board, -beta, -alpha, depth-1);

        start = high_resolution_clock::now();
        board.pop();
        stop = high_resolution_clock::now();
        metrics["Pop time"] += (stop - start).count() * 1e-9;

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int Ai::negaMax(Board board, int alpha, int beta, int depth, Move* bestMovePtr) {
    if (depth == 0 || board.legalMoves.size() == 0) {
        auto start = high_resolution_clock::now();
        int score = quiesce(board, alpha, beta, maxQuiesceDepth);
        auto stop = high_resolution_clock::now();
        metrics["Quiesce time"] += (stop - start).count() * 1e-9;
        return score;
    }

    int alphaOrig = alpha;

    TTEntry entry;
    if (transpositionTable.getEntry(board.zobrist, &entry)) {
        if (entry.depth >= depth) {
            metrics["TT use"]++;
            if (entry.flag == EXACT) {
                if (bestMovePtr != nullptr) *bestMovePtr = entry.bestMove;
                return entry.score;
            } else if (entry.flag == ALPHA) {
                if (entry.score > alpha) alpha = entry.score;
            } else if (entry.flag == BETA) {
                if (entry.score < beta) beta = entry.score;
            }

            if (alpha >= beta) {
                if (bestMovePtr != nullptr) *bestMovePtr = entry.bestMove;
                return entry.score;
            }
        }
    }


    int score=std::numeric_limits<int>::min();
    Move bestMove;

    std::vector<std::pair<Move, int>> legalMoves = orderMove(board, board.legalMoves);

    for (auto moveScore: legalMoves) {
        Move move = moveScore.first;
        if (move.defend) continue;
        /* std::cout << "NegaMax " << bitboardToSquareName(move.from) << " to " <<  bitboardToSquareName(move.to) << ", depth " << depth << "\n"; */

        auto start = high_resolution_clock::now();
        board.push(move);
        auto stop = high_resolution_clock::now();
        metrics["Push time"] += (stop - start).count() * 1e-9;

        score = -negaMax(board, -beta, -alpha, depth-1, nullptr);

        start = high_resolution_clock::now();
        board.pop();
        stop = high_resolution_clock::now();
        metrics["Pop time"] += (stop - start).count() * 1e-9;

        if (score > alpha) {
            bestMove = move;
            alpha = score;
        }
        if (alpha >= beta) {
            bestMove = move;
            break;
        }
    }

    if (bestMovePtr != nullptr) *bestMovePtr = bestMove;

    if (score <= alphaOrig) {
        transpositionTable.setEntry(board.zobrist, bestMove, depth, score, BETA);
    } else if (score >= beta) {
        transpositionTable.setEntry(board.zobrist, bestMove, depth, score, ALPHA);
    } else {
        transpositionTable.setEntry(board.zobrist, bestMove, depth, score, EXACT);
    }

    return alpha;
}

std::vector<std::pair<Move, int>> Ai::orderMove(Board board, std::vector<Move> moves) {
    auto start = high_resolution_clock::now();
    std::vector<std::pair<Move, int>> movesList;

    for (Move move: moves) {
        if (timeLimitExceded) break;
        int score = 0;

        Piece attackedPiece;
        Piece attackingPiece;
        board.pieceAtBitboard(move.from, &attackingPiece);
        if (board.pieceAtBitboard(move.to, &attackedPiece) && !move.defend)
            score += 10 * (getPieceValue(attackedPiece.pieceType) - getPieceValue(attackingPiece.pieceType));
        
        if (move.promotion) score += getPieceValue(Queen);

        for (Move opponentMove: board.opponentLegalMoves)
            if (opponentMove.to == move.to && move.attack && !move.defend) {
                Piece opponentPiece;
                board.pieceAtBitboard(opponentMove.from, &opponentPiece);
                if (opponentPiece.pieceType == Pawn) {
                    score -= getPieceValue(attackingPiece.pieceType);
                    break;
                }
            }

        movesList.push_back(std::make_pair(move, score));
    }

    std::sort(movesList.begin(), movesList.end(), [&](auto a, auto b) {
            return a.second > b.second;
    });

    auto stop = high_resolution_clock::now();
    metrics["Order time"] += (stop - start).count() * 1e-9;
    return movesList;
}

Move Ai::play(Board board) {
    timeLimitExceded = false;

    Move bestMove;
    bestMove.from = 0;
    bestMove.to = 0;

    cleanMetrics();

    moveStartTime = high_resolution_clock::now();
    int depth = 0;
    while (depth < maxDepth) {
        depth++;
        Move move;
        move.from = 0;
        move.to = 0;

        negaMax(board, std::numeric_limits<int>::min() + 1, std::numeric_limits<int>::max() - 1, depth, &move);
        auto stop = high_resolution_clock::now();

        if (timeLimitExceded) break;

        bestMove = move;
        metrics["Last depth"] = depth;
        metrics["Last depth time"] = (stop-moveStartTime).count() * 1e-9;
    }

    return bestMove;
}

void Ai::freeMemory() {
    transpositionTable.freeMemory();
}

void Ai::cleanMetrics() {
    for (auto metric: metrics)
        metrics[metric.first] = 0;
}
