#include "main.h"

SDL_Texture *textureMap;

SDL_Texture *LoadTexture(char *filePath, SDL_Renderer *renderTarget) {
    SDL_Texture *texture;
    SDL_Surface *surface = SDL_LoadBMP(filePath);
    if (surface == NULL) {
        printf("Could not load %s\n", filePath);
        return NULL;
    } 
    texture = SDL_CreateTextureFromSurface(renderTarget, surface);
    if (texture == NULL) {
        printf("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(surface);
    return texture;
}

void DrawTile(int tile, int x, int y, SDL_Renderer *renderer) {
    
    SDL_Rect srcrect;
    srcrect.x = tile * TILE_SIZE;
    srcrect.y = 0;
    srcrect.w = TILE_SIZE;
    srcrect.h = TILE_SIZE;

    SDL_Rect dstrect;
    dstrect.x = x * TILE_SIZE;
    dstrect.y = y * TILE_SIZE;
    dstrect.w = TILE_SIZE;
    dstrect.h = TILE_SIZE;
    
    SDL_RenderCopy(renderer, textureMap, &srcrect, &dstrect);
}

void LoadTextureMap(SDL_Renderer *renderer) {
    textureMap = LoadTexture("res/textureMap.bmp", renderer);
}

void UnloadTextureMap() {
    SDL_DestroyTexture(textureMap);
}
