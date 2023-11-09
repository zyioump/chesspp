#include "user_interface.hpp"

UserInterface::UserInterface() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow("Chesspp", 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowSize[0], windowSize[1], 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Surface* piece_set = IMG_Load("res/piece_set.png");
    piece_set_texture = SDL_CreateTextureFromSurface(renderer, piece_set);
    SDL_FreeSurface(piece_set);

    int textureWidth;
    SDL_QueryTexture(piece_set_texture, NULL, NULL, &textureWidth, NULL);
    piece_texture_size = textureWidth / 6;
}

void UserInterface::displayBoard(Board board) {
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            if ((i+j) % 2 == 0)
                SDL_SetRenderDrawColor(renderer, 0xB5, 0x88, 0x63, 255);
            else 
                SDL_SetRenderDrawColor(renderer, 0xF0, 0xD9, 0xB5, 255);

            SDL_Rect rect = {i*squareSize, j*squareSize, squareSize, squareSize};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    for (int color=0; color<=1; color++) {
        for (int piece_type=0; piece_type<6; piece_type++){
            std::unordered_set<uint64_t> piece_bitboards = board.pieces((PieceType) piece_type, (Color) color);
            for (uint64_t bitboard : piece_bitboards) {
                uint square = log2(bitboard);
                int col = square % 8;
                int row = (square - col) / 8;

                SDL_Rect srcRect = {piece_type*piece_texture_size, color*piece_texture_size, piece_texture_size, piece_texture_size};
                SDL_Rect dstRect = {col*squareSize, (7-row)*squareSize, squareSize, squareSize};

                SDL_RenderCopy(renderer, piece_set_texture, &srcRect, &dstRect);
            }
        }
    }
    
    if (highlighted_square >= 0) {
        uint i = highlighted_square%8;
        uint j = (7 - (highlighted_square - i) / 8);

        filledCircleRGBA(renderer, (i+0.5) * squareSize, (j+0.5) * squareSize, squareSize/8, 200, 200, 200, 255);

        std::unordered_set<uint>::iterator ite;
        for (ite=highlighted_moves.begin(); ite!=highlighted_moves.end(); ite++){
            i = *ite%8;
            j = (7 - (*ite - i) / 8);
            filledCircleRGBA(renderer, (i+0.5) * squareSize, (j+0.5) * squareSize, squareSize/6, 200, 200, 200, 255);
        }
    }
    
    SDL_RenderPresent(renderer);
}

UIFlag UserInterface::play(Board board, Move* move) {
    displayBoard(board);
    while (true) {
        SDL_Event e;
        if (SDL_WaitEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return QUIT;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT){
                    uint i = e.button.x / squareSize;
                    uint j = 7 - (e.button.y / squareSize);
                    uint square = i + 8*j;

                    if (highlighted_moves.find(square) != highlighted_moves.end()) {
                        move->from = (uint64_t) 1 << highlighted_square;
                        move->to = (uint64_t) 1 << square;
                        clearHighlight();

                        return MOVE;
                    }

                    clearHighlight();
                    Piece piece;
                    if (board.pieceAt(square, &piece)){
                        highlighted_square = square;

                        std::unordered_set<Move, MoveHash>::iterator ite;

                        for (ite = board.legalMoves.begin(); ite != board.legalMoves.end(); ite++) {
                            if (log2(ite->from) == square && !ite->defend) highlighted_moves.insert(log2(ite->to));
                        }
                    }
                } else clearHighlight();
                displayBoard(board);
            }
        }
    }
    return NONE;
}

void UserInterface::clearHighlight() {
    highlighted_square = -1;
    highlighted_moves.clear();
}

void UserInterface::free_memory() {
    SDL_DestroyTexture(piece_set_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
	SDL_Quit();
}
