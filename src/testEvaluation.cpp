#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "board.hpp"
#include "ai.hpp"

int main() {
    std::ifstream chessDataFile("res/chessData.csv");
    if (chessDataFile.is_open()) {
        std::string chessDataLine;
        std::string fen;
        int score;
        int lineRead = 0;
        int totalLine = 100000;

        float squareError = 0;

        float evaluationTime = 0;

        Ai ai;
        while (chessDataFile && lineRead <= totalLine) {
            std::getline(chessDataFile, chessDataLine);
            lineRead++;
            if (lineRead == 1) continue;

            size_t commaIndex =chessDataLine.find(',');
            fen = chessDataLine.substr(0, commaIndex);

            // divided by two to normalize with chess com
            score = std::stoi(chessDataLine.substr(commaIndex+1)) / 2;
            
            Board board(fen);
            auto start = std::chrono::high_resolution_clock::now();
            auto evaluation = ai.evaluate(board);
            auto stop = std::chrono::high_resolution_clock::now();

            evaluationTime += (stop - start).count() * 1e-3;

            squareError += pow(evaluation.first - score, 2);
        }

        std::cout << "RMSE : " << sqrt(squareError/totalLine) << ", Evaluation time : " << evaluationTime/totalLine << "us\n";
    }
    return 0;
}
