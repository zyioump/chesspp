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
    std::list<Move> bestMoveVariation;

    Move* lastBestMove = nullptr;

    if (lastVariation != nullptr) if (lastVariation->size() > 0) {
        lastBestMove = &lastVariation->front();
        lastVariation->pop_front();
    }

    std::vector<std::pair<Move, int>> legalMoves = orderMove(board, board.legalMoves, lastBestMove);

    for (auto moveScore: legalMoves) {
        if (stopSearching) break;

        Move move = moveScore.first;
        if (move.defend) continue;
        /* std::cout << "NegaMax " << bitboardToSquareName(move.from) << " to " <<  bitboardToSquareName(move.to) << ", depth " << depth << "\n"; */

        auto start = high_resolution_clock::now();
        board.push(move);
        auto stop = high_resolution_clock::now();
        metrics["Push time"] += (stop - start).count() * 1e-9;

        std::list<Move> moveVariation = {};
        std::list<Move>* moveLastVariation = nullptr;
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

std::vector<std::pair<Move, int>> Ai::orderMove(Board board, std::vector<Move> moves, Move* lastBestMove) {
    auto start = high_resolution_clock::now();
    std::vector<std::pair<Move, int>> movesList;

    for (Move move: moves) {
        int score = 0;
        
        if (lastBestMove != nullptr) {
            if (*lastBestMove == move) {
                score = std::numeric_limits<int>::max()-1;
                movesList.push_back(std::make_pair(move, score));
                continue;
            }
        }

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

