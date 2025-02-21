#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int initialize_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return ERROR;
    }   return OK;
}

SDL_Window* gsdl_windowNew(const char *title, int width, int height) {
    SDL_Window *window = SDL_CreateWindow(
        title,                  // Title of the window
        SDL_WINDOWPOS_CENTERED, // X position of the window
        SDL_WINDOWPOS_CENTERED, // Y position of the window
        width,                  // Window width
        height,                 // Window height
        SDL_WINDOW_SHOWN        // Window flags (can include SDL_WINDOW_RESIZABLE, etc.)
    );
    if (window == NULL) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();  // Clean up SDL if window creation fails
        return NULL;  // Return NULL to indicate failure
    }   return window;
}

SDL_Renderer *gsdl_newRenderer(SDL_Window *parentWindow, int rendererFlags){
    SDL_Renderer *renderer = SDL_CreateRenderer(parentWindow, -1, rendererFlags);
    if (!renderer) { printf("SDL_CreateRenderer error: %s\n", SDL_GetError()); SDL_Quit(); return NULL; }
    return renderer;
}

void gsdl_windowDestroy(SDL_Window *window) {
    if (window) { SDL_DestroyWindow(window); }
}

void gsdl_rendererDestroy(SDL_Renderer *renderer) {
    if (renderer) { SDL_DestroyRenderer(renderer); }
}

bool gsdl_setColor(SDL_Renderer *renderer, char red, char green, char blue, char alpha) { return SDL_SetRenderDrawColor(renderer, red, green, blue, alpha); }
void gsdl_clear(SDL_Renderer *renderer) { SDL_RenderClear(renderer); SDL_RenderPresent(renderer); }
bool gsdl_pollEvent(SDL_Event *event) { return SDL_PollEvent(event); }
void gsdl_delay(int milliseconds) { SDL_Delay(milliseconds); }

SDL_Texture *gsdl_loadTexture(SDL_Renderer *renderer, const char *filename) { 
    SDL_Texture *ref = IMG_LoadTexture(renderer, filename);
    if ( ref == NULL ) { printf("SDL_loadTexture error: %s\n", SDL_GetError()); return NULL; } // don't quit; we can recover from these types of errors
    return ref;
}