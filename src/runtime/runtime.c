#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define MOONGIN
#define OK 0
#define ERROR -1
#define RET_ERR return 1

%s // runtime stb
%s // luac payload

int main (int argc, char **argv) {
    lua_State *globalState = glua_initFunctions();
    int code = glua_byterun(globalState, lua_entry, sizeof(lua_entry));
}
