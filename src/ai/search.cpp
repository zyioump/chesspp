#include <ai.hpp>

using std::chrono::high_resolution_clock;

int Ai::negaMax(Board board, int alpha, int beta, int depth, Move* bestMovePtr, std::list<Move>* bestVariation, std::list<Move>* lastVariation) {
    if (depth == 0 || board.legalMoves.size() == 0) {
        auto start = high_resolution_clock::now();
        int score = quiesce(board, alpha, beta, maxQuiesceDepth);
        auto stop = high_resolution_clock::now();
        metrics["Quiesce time"] += (stop - start).count() * 1e-9;
        return score;
    }

    int alphaOrig = alpha;

    bool futilityEnable = !board.moveStack.back().lastMoveIsCapture && !board.inCheck && depth == 1;
    if (futilityEnable) {
        auto start = high_resolution_clock::now();
        auto futilityEvaluation = evaluate(board);
        auto stop = high_resolution_clock::now();
        metrics["Evaluation num"]++;
        metrics["Evaluation time"] += (stop - start).count() * 1e-9;

        futilityEnable = futilityEvaluation.first + getPieceValue(Bishop) <= alpha;
    }

    TTEntry entry;
    Move* hashMove = nullptr;
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
            hashMove = &entry.bestMove;

            if (alpha >= beta) {
                if (bestMovePtr != nullptr) *bestMovePtr = entry.bestMove;
                return entry.score;
            }
        }
    }

    int score=std::numeric_limits<int>::min();
    Move bestMove;
    std::list<Move> bestMoveVariation;

    Move* lastBestMove = nullptr;

    if (lastVariation != nullptr) if (lastVariation->size() > 0) {
        lastBestMove = &lastVariation->front();
        lastVariation->pop_front();
    }

    std::vector<std::pair<Move, int>> legalMoves = orderMove(board, board.legalMoves, lastBestMove, hashMove);

    Move move;
    std::list<Move>* moveLastVariation = nullptr;
    std::list<Move>  moveVariation;

    for (auto moveScore: legalMoves) {
        if (stopSearching) break;

        move = moveScore.first;
        if (move.defend) continue;
        /* std::cout << "NegaMax " << bitboardToSquareName(move.from) << " to " <<  bitboardToSquareName(move.to) << ", depth " << depth << "\n"; */

        auto start = high_resolution_clock::now();
        board.push(move);
        auto stop = high_resolution_clock::now();
        metrics["Push time"] += (stop - start).count() * 1e-9;

        if (futilityEnable && !board.inCheck) {
            start = high_resolution_clock::now();
            board.pop();
            stop = high_resolution_clock::now();
            metrics["Pop time"] += (stop - start).count() * 1e-9;
            continue;
        }

        moveVariation.clear();
        moveLastVariation = nullptr;
        if (lastBestMove != nullptr) if (move == *lastBestMove) moveLastVariation = lastVariation;
        score = -negaMax(board, -beta, -alpha, depth-1, nullptr, &moveVariation, moveLastVariation);

        start = high_resolution_clock::now();
        board.pop();
        stop = high_resolution_clock::now();
        metrics["Pop time"] += (stop - start).count() * 1e-9;

        if (score > alpha) {
            bestMove = move;
            bestMoveVariation = moveVariation;
            alpha = score;
        }
        if (alpha >= beta) {
            bestMove = move;
            bestMoveVariation = moveVariation;
            break;
        }
    }

    if (score==std::numeric_limits<int>::min()) {
        if (futilityEnable) {
            metrics["Futility skip"]++;
            auto start = high_resolution_clock::now();
            score = quiesce(board, alpha, beta, maxQuiesceDepth);
            auto stop = high_resolution_clock::now();
            metrics["Quiesce time"] += (stop - start).count() * 1e-9;
            return score;
        } else return 0;
    }

    if (bestMovePtr != nullptr) *bestMovePtr = bestMove;
    bestVariation->insert(bestVariation->end(), bestMoveVariation.begin(), bestMoveVariation.end());
    bestVariation->push_front(bestMove);

    if (score <= alphaOrig) {
        transpositionTable.setEntry(board.zobrist, bestMove, depth, score, BETA);
    } else if (score >= beta) {
        transpositionTable.setEntry(board.zobrist, bestMove, depth, score, ALPHA);
    } else {
        transpositionTable.setEntry(board.zobrist, bestMove, depth, score, EXACT);
    }

    return alpha;
}

std::vector<std::pair<Move, int>> Ai::orderMove(Board board, std::vector<Move> moves, Move* lastBestMove, Move* hashMove) {
    auto start = high_resolution_clock::now();
    std::vector<std::pair<Move, int>> movesList;

    Piece attackedPiece;
    Piece attackingPiece;
    int score;
    for (Move move: moves) {
        score = 0;
        
        if (lastBestMove != nullptr) {
            if (*lastBestMove == move) {
                score = std::numeric_limits<int>::max()-1;
                movesList.push_back(std::make_pair(move, score));
                continue;
            } 
        }
        if (hashMove != nullptr) {
            if (*hashMove == move) {
                score = std::numeric_limits<int>::max()-2;
                movesList.push_back(std::make_pair(move, score));
                continue;
            } 
        }

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

