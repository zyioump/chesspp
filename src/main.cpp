#include "user_interface.hpp"
#include "board.hpp"
#include "ai.hpp"

int main(int argc, char *argv[]) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    /* Board board("8/5q2/6n1/8/5P2/8/6P1/2B5 w - - 0 1"); */
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

        board.push(ai.play(board));
    }

    ui.freeMemory();
    ai.freeMemory();
    return 0;
}

