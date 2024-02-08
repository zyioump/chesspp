#include "board.hpp"
#include "utils.hpp"

#include <chrono>

using std::chrono::high_resolution_clock;

int perft(Board board, int depth) {
    if (depth == 0) return 1;

    int node = 0;
    auto legalMoves = board.legalMoves;
    for (Move move: legalMoves) {
        /* std::cout << depth << " " << bitboardToSquareName(move.from) << " " << bitboardToSquareName(move.to) << "\n"; */
        board.push(move);

        node += perft(board, depth-1);
        board.pop();
    }
    return node;
}

void performPerft(std::string fen) {
    Board board(fen);

    printf("===== Perft \"%s\" =====\n", fen.c_str());
    printf("Depth\tNode nb\tTime (s)\n");

    int totalNode = 0;
    float totalTime = 0;

    for (int depth=1; depth<=4; depth++) {
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

int main(int argc, char *argv[]) {
    performPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    performPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    performPerft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    performPerft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
}

