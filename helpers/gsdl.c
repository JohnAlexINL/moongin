#include <stdio.h>
#include <SDL2/SDL.h>

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

void gsdl_windowClean(SDL_Window *window) {
    if (window) { SDL_DestroyWindow(window); }
}