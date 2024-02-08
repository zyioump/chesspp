#include "user_interface.hpp"
#include "board.hpp"
#include "ai.hpp"

int main(int argc, char *argv[]) {
    Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    /* Board board("8/8/8/8/k3p2R/8/3P4/4K3 w - - 0 1"); */
    /* Board board("8/8/8/8/1b6/2P5/8/4K3 w - - 0 1"); */
    /* Board board("2r2rk1/pp1n1pp1/1q3b1p/2pp2PP/2P5/Q4N2/PP1B1P2/1K1R3R w - - 0 20 "); */ 
    UserInterface ui;
    Ai ai;

    ui.displayBoard(board, ai);
    while (true) {
        Move move;
        UIFlag flag = ui.play(board, ai, &move);
        if (flag == QUIT) break;
        else if (flag == MOVE) {
            bool result = board.push(move);
            if (!result) printf("Error while playing move\n");
        }
        else if (flag == POP) {
            board.pop();
            continue;
        }
        else if (flag == NONE) continue;
        ui.displayBoard(board, ai);
    }

    ui.freeMemory();
    ai.freeMemory();
    return 0;
}

