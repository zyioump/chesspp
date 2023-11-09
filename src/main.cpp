#include "user_interface.hpp"
#include "board.hpp"

int main(int argc, char *argv[]) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    UserInterface ui;

    board.pieces(Rook, White);

    while (true) {
        Move move;
        UIFlag flag = ui.play(board, &move);
        if (flag == QUIT) break;
        if (flag == MOVE) board.push(move);
    }

    ui.free_memory();
    return 0;
}

