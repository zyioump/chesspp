#include "user_interface.hpp"
#include "board.hpp"
#include "ai.hpp"

#include <future>
#include <chrono>

int main(int argc, char *argv[]) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    /* Board board("6kr/8/8/8/8/8/8/1K6 w - - 0 1"); */
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
            board.push(move);
        }
        else if (flag == POP) {
            board.pop();
            continue;
        }
        else if (flag == NONE) continue;
        ui.displayBoard(board, ai);

        std::future<Move> futureMove = std::async(&Ai::play, &ai, board);
        ai.lock = true;

        auto start = std::chrono::high_resolution_clock::now();
        while (ai.lock) {
            flag = ui.play(board, ai, &move);
            if (flag == QUIT) {
                ai.stopSearching = true;
                break;
            };

            auto currentTime = std::chrono::high_resolution_clock::now();
            if ((currentTime - start).count()*1e-9 >= 0.25) {
                start = currentTime;
                ui.displayBoard(board, ai);
            }
        }
        if (ai.lock) break;

        board.push(futureMove.get());
        ui.displayBoard(board, ai);
    }

    ui.freeMemory();
    ai.freeMemory();
    return 0;
}

