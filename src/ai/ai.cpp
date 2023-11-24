#include <ai.hpp>

using std::chrono::high_resolution_clock;

Move Ai::play(Board board) {
    lock = true;
    stopSearching = false;

    Move bestMove;
    bestMove.from = 0;
    bestMove.to = 0;

    cleanMetrics();

    std::list<Move> bestVariation = {};

    moveStartTime = high_resolution_clock::now();
    int depth = 0;
    while (depth < maxDepth) {
        depth++;
        Move move;
        move.from = 0;
        move.to = 0;

        std::list<Move> variation = {};

        negaMax(board, std::numeric_limits<int>::min() + 1, std::numeric_limits<int>::max() - 1, depth, &move, &variation, &bestVariation);
        auto stop = high_resolution_clock::now();

        /* for (Move move: variation) { */
        /*     std::cout << bitboardToSquareName(move.from) << " " << bitboardToSquareName(move.to) << " - "; */
        /* } */
        /* std::cout << "\n"; */

        if (stopSearching) break;

        float lastDepthTime = (stop-moveStartTime).count() * 1e-9;
        bestMove = move;
        bestVariation = variation;
        metrics["Last depth"] = depth;
        metrics["Last depth time"] = lastDepthTime;
    }

    auto stop = high_resolution_clock::now();
    metrics["Total time"] = (stop-moveStartTime).count() * 1e-9;

    lock = false;
    return bestMove;
}

void Ai::freeMemory() {
    transpositionTable.freeMemory();
}

void Ai::cleanMetrics() {
    for (auto metric: metrics)
        metrics[metric.first] = 0;
}
