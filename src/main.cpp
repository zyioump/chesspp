#include "user_interface.hpp"
#include "board.hpp"
#include "ai.hpp"

int main(int argc, char *argv[]) {
    /* Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); */
    Board board("8/8/8/8/8/8/2K3q1/8 w - - 0 1");
    /* Board board("8/8/8/8/1b6/2P5/8/4K3 w - - 0 1"); */
    /* Board board("2r2rk1/pp1n1pp1/1q3b1p/2pp2PP/2P5/Q4N2/PP1B1P2/1K1R3R w - - 0 20 "); */ 
    UserInterface ui;
    Ai ai;

    board.pieces(Rook, White);

    while (true) {
        Move move;
        UIFlag flag = ui.play(board, ai, &move);
        if (flag == QUIT) break;
        else if (flag == MOVE) board.push(move);
        else if (flag == POP) {
            board.pop();
            continue;
        }
        else if (flag == NONE) continue;

        /* board.push(ai.play(board)); */
    }

    ui.freeMemory();
    ai.freeMemory();
    return 0;
}

