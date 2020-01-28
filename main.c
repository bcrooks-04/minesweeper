#include "main.h"
#include "graphics.h"


bool failed;
int tilesLeft;

struct tile
{
    bool isBomb;
    bool flagged;
    bool searched;
    uint8_t type;
    uint8_t nearbyBombCount;
};


void SetTileTypes(struct tile *board) {
    for (int y = 0; y < ROWS; y++) 
        for (int x = 0; x < COLUMNS; x++) {
            switch (board[y * COLUMNS + x].searched)
            {
            case true:
                if (board[y * COLUMNS + x].isBomb) {
                    board[y * COLUMNS + x].type = 11;
                }
                else {
                    board[y * COLUMNS + x].type = board[y * COLUMNS + x].nearbyBombCount;
                }
                break;
            
            case false:
                if (board[y * COLUMNS + x].flagged) {
                    board[y * COLUMNS + x].type = 10;
                }
                else {
                    board[y * COLUMNS + x].type = 9;
                }
                break;
            }
        }
}

void UpdateGraphics(SDL_Renderer *renderer, struct tile *board) {
    SetTileTypes(board);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);       
    
    // Draw the board
    for (int y = 0; y < ROWS; y++) 
        for (int x = 0; x < COLUMNS; x++) {
            DrawTile(board[y * COLUMNS + x].type, x, y, renderer);
        }
    
    SDL_RenderPresent(renderer);
}

void RevealBombs(struct tile *board) {
    failed = true;
    for (int y = 0; y < ROWS; y++) 
        for (int x = 0; x < COLUMNS; x++) {
            if (board[y * COLUMNS + x].isBomb) board[y * COLUMNS + x].searched = true;
        }
}

void RevealTile(int x, int y, struct tile *board) {
    
    if (board[y * COLUMNS + x].flagged == true) {
        board[y * COLUMNS + x].flagged = false;
        return;
    }

    if (board[y * COLUMNS + x].searched == true) return;

    if (y < 0)          return;
    if (x < 0)          return;
    if (y >= ROWS)      return;
    if (x >= COLUMNS)   return;

    board[y * COLUMNS + x].searched = true;
    if (board[y * COLUMNS + x].isBomb) {
        RevealBombs(board);
    }

    tilesLeft --;
    if (tilesLeft <= 0) {
        printf("Winner!");
    }

    if (!board[y * COLUMNS + x].isBomb && board[y * COLUMNS + x].nearbyBombCount == 0) {
        
        RevealTile(x - 1, y, board);
        RevealTile(x + 1, y, board);
        RevealTile(x, y - 1, board);
        RevealTile(x, y + 1, board);

        RevealTile(x - 1, y - 1, board);
        RevealTile(x + 1, y - 1, board);
        RevealTile(x - 1, y + 1, board);
        RevealTile(x + 1, y + 1, board);
    }
}

void FlagTile(int x, int y, struct tile *board) {
    if (board[y * COLUMNS + x].flagged == true) {
        board[y * COLUMNS + x].flagged = false;
        return;
    }
    board[y * COLUMNS + x].flagged = true;
}

void NewGame(SDL_Renderer *renderer,struct tile *board) {
    // Set the board tiles 
    for (int y = 0; y < ROWS; y++) 
        for (int x = 0; x < COLUMNS; x++) {
            board[y * COLUMNS + x].isBomb = false;
            board[y * COLUMNS + x].flagged = false;
            board[y * COLUMNS + x].searched = false;
        }

    // Place bombs
    for (int i = 0; i < NUMBER_OF_BOMBS; i++) {
        bool bombPlaced = false;
        while (!bombPlaced) {
            int x = rand() % ROWS;
            int y = rand() % COLUMNS;

            if (board[y * COLUMNS + x].isBomb == false) {
                board[y * COLUMNS + x].isBomb = true;
                bombPlaced = true;
            }
        }
    }

    // Check all tiles for nearby bombs
    for (int y = 0; y < ROWS; y++) 
        for (int x = 0; x < COLUMNS; x++) {
            int bombCount = 0;

            if (board[y * COLUMNS + x].isBomb) 
                continue;

            for (int i = -1; i <= 1; i++) 
                for (int j = -1; j <= 1; j++) {
                    if (board[(y + i) * COLUMNS + (x + j)].isBomb == true) {
                        if (y + i < 0)          continue;
                        if (x + j < 0)          continue;
                        if (y + i >= ROWS)      continue;
                        if (x + j >= COLUMNS)   continue;
                        bombCount ++;
                    }
                }
            
            board[y * COLUMNS + x].nearbyBombCount = bombCount;
            
        }
    UpdateGraphics(renderer, board);
    failed = false;
    tilesLeft = COLUMNS * ROWS - NUMBER_OF_BOMBS;
}

int main(int argc, char **argv) {

    // SDL SETUP-----------------------------------------------------------------------------------
    // Window and renderer
    SDL_Window *window;
    int windowWidth = COLUMNS * TILE_SIZE;
    int windowHeight = ROWS * TILE_SIZE;

    SDL_Surface *surface;

    SDL_Renderer *renderer;

    // Initialise SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    else {
        printf("SDL_Init was successfull!\n");
    }

    // Create the window
    window = SDL_CreateWindow(
        "Minesweeper",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        windowWidth,
        windowHeight,
        0
    );
    if (window == NULL) {
        printf("SDL_CreateWindow failed: %s", SDL_GetError());
        return 1;
    }

    // Create the window surface
    surface = SDL_GetWindowSurface(window);

    // Create the renderer
    renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load resources
    LoadTextureMap(renderer);





    // GAMEPLAY------------------------------------------------------------------------------------
    
    // Game start
    srand(time(0)); // Seed the random number generator
    struct tile board[COLUMNS * ROWS];

    NewGame(renderer, board);
    

    bool running = true;
    bool firstClick = true;
    int mouseTileX, mouseTileY;

    // Game loop
    while (running) {

        // Event Handling
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                printf("Quitting application\n");
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_r) {
                    NewGame(renderer, board);
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                mouseTileX = x / TILE_SIZE;
                mouseTileY = y / TILE_SIZE;
            }
            if (!failed) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        RevealTile(mouseTileX, mouseTileY, board);
                        UpdateGraphics(renderer, board);
                    }

                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        FlagTile(mouseTileX, mouseTileY, board);
                        UpdateGraphics(renderer, board);
                    }
                }
            }
        }
    }
    

    // Quit SDL and cleanup
    printf("Application is now ending");
    UnloadTextureMap();
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    
    return 0;
}