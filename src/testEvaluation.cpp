#include <iostream>
#include <fstream>
#include <string>

#include "board.hpp"
#include "ai.hpp"

int main() {
    std::ifstream chessDataFile("res/chessData.csv");
    if (chessDataFile.is_open()) {
        std::string chessDataLine;
        std::string fen;
        int score;
        int lineRead = 0;
        int totalLine = 1000;

        float squareError = 0;

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
            auto evaluation = ai.evaluate(board);

            squareError += pow(evaluation.first - score, 2);
        }

        std::cout << "RMSE : " << sqrt(squareError/totalLine) << "\n";
    }
    return 0;
}
