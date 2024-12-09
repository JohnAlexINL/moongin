#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>
#define OK 0
#define ERROR -1
// include "gsdl.c"
%s
// include "glua.c"
%s
// include "main.lua"
%s

int main (int argc, char **argv) {
    lua_State *globalState = luaL_newstate();
    luaL_openlibs(globalState);
    int code = glua_byterun(globalState, lua_entry, sizeof(lua_entry));
}