#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define OK 0
#define ERROR -1
// include "list.c"
%s
// include "item.c"
%s
// include "gsdl.c"
%s
// include "glua.c"
%s
// include "main.lua"
%s

int main (int argc, char **argv) {
    lua_State *globalState = glua_initFunctions();
    int code = glua_byterun(globalState, lua_entry, sizeof(lua_entry));
}