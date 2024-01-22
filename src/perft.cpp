#include "board.hpp"
#include "utils.hpp"

#include <chrono>

using std::chrono::high_resolution_clock;

int perft(Board board, int depth) {
    if (depth == 0) return 1;

    int node = 0;
    auto legalMoves = board.legalMoves;
    for (Move move: legalMoves) {
        if (move.defend) continue;
        /* std::cout << depth << " " << bitboardToSquareName(move.from) << " " << bitboardToSquareName(move.to) << "\n"; */
        board.push(move);

        node += perft(board, depth-1);
        board.pop();
    }
    return node;
}

int main(int argc, char *argv[]) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    printf("===== Perft =====\n");
    printf("Depth\tNode nb\tTime (s)\n");

    int totalNode = 0;
    float totalTime = 0;

    for (int depth=0; depth<=4; depth++) {
        auto start = high_resolution_clock::now();
        int nodeNb = perft(board, depth);
        totalNode += nodeNb;
        auto end = high_resolution_clock::now();

        float time = (end - start).count() * 1e-9;
        totalTime += time;

        printf("%d\t%d\t%f\n", depth, nodeNb, time);
    }

    printf("Nodes\t%d\nTime\t%f\nNPS\t%f\n", totalNode, totalTime, float(totalNode)/totalTime);
}

