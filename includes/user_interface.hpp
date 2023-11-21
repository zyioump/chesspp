#ifndef UserInterface_H
#define UserInterface_H

#include <unordered_set>
#include <math.h>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <iomanip>

#include "board.hpp"
#include "ai.hpp"

enum UIFlag {
    QUIT,
    MOVE,
    POP,
    NONE
};

class UserInterface {
    private:
        int chessSize = 800;
        int debugSize = 800;
        int margin = 10;
        int windowSize[2] = {chessSize + debugSize, chessSize};
        int squareSize = chessSize / 8;
        int piece_texture_size;
        int highlighted_square = -1;
        std::unordered_set<uint> highlighted_moves;
        void clearHighlight();
        void displayDebug(Board board, Ai ai);
        SDL_Rect drawCastling(Board board, int x, int y);
        SDL_Rect drawMetrics(std::map<std::string, float> metrics, int x);

        TTF_Font* font;
        SDL_Color textColor = {255, 255, 255};

        UIFlag chessClick(SDL_Event e, Board board, Move* move);
        SDL_Rect writeText(std::string text, int x, int y, SDL_Color color);

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* piece_set_texture;

    public:
        UserInterface();
        void displayBoard(Board board, Ai ai);
        UIFlag play(Board board, Ai ai, Move* move);
        void freeMemory();
};
#endif
