#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>
// include "gsdl.c"
%s
// include "glua.c"
%s
// include "main.lua"
%s
int main (int argc, char **argv) {
    printf(lua_entry);
}