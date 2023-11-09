#ifndef UserInterface_H
#define UserInterface_H

#include <unordered_set>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "board.hpp"
#include <math.h>

enum UIFlag {
    QUIT,
    MOVE,
    NONE
};

class UserInterface {
    private:
        int windowSize[2] = {800, 800};
        int squareSize = windowSize[0] / 8;
        int piece_texture_size;
        int highlighted_square = -1;
        std::unordered_set<uint> highlighted_moves;
        void clearHighlight();

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* piece_set_texture;

    public:
        UserInterface();
        void displayBoard(Board board);
        UIFlag play(Board board, Move* move);
        void free_memory();
};
#endif
