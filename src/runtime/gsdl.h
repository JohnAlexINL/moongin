#ifndef MOONGIN 
    #include "includes.h"
#endif

int gsdl_init();
void gsdl_quit();

SDL_Window* gsdl_windowNew(const char *title, int width, int height, int flags);
SDL_Renderer *gsdl_newRenderer(SDL_Window *parentWindow, int rendererFlags);
void gsdl_windowDestroy(SDL_Window *window);
void gsdl_rendererDestroy(SDL_Renderer *renderer);
void gsdl_presentRenderer(SDL_Renderer *renderer);
bool gsdl_setColor(SDL_Renderer *renderer, char red, char green, char blue, char alpha);
void gsdl_clear(SDL_Renderer *renderer);
void gsdl_delay(int milliseconds);
SDL_Texture *gsdl_loadTexture(SDL_Renderer *renderer, const char *filename);
int gsdl_renderTexture(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect src, SDL_Rect dest);
bool gsdl_pollEvent();

// ================================ GSDL Constant Objects

char pushfmtstr[255];
const SDL_Rect BadBox;
SDL_Event eventHandle;
