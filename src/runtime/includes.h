#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MOONGIN
    #include "list.c"
    #include "item.c"
    #include "gsdl.h"
    #include "glua.h"
    #include "events.h"
    #define OK 0
    #define ERROR -1
    #define RET_ERR return 1
