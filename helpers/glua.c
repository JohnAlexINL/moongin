#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>

list_t *glua_windows;

int glua_byterun(lua_State *context, const char *bytecode, int bytecode_size) {
    if (luaL_loadbuffer(context, bytecode, bytecode_size, "chunk") != LUA_OK) {
        printf("Error loading main function: %s\n", lua_tostring(context, -1));
        return ERROR;
    }
    if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK) {
        printf("Error running main function: %s\n", lua_tostring(context, -1));
        return ERROR;
    }
    return OK;
}

int glua_newWindow(lua_State *context){
    if (!lua_isstring(context, 1))  { lua_pushstring(context, "sdl_newWindow expected string title for argument 1"); lua_error(context); }
    if (!lua_isinteger(context, 2)) { lua_pushstring(context, "sdl_newWindow expected int width for argument 1"); lua_error(context); }
    if (!lua_isinteger(context, 3)) { lua_pushstring(context, "sdl_newWindow expected int height for argument 2"); lua_error(context); }
    const char *title = lua_tostring(context, 1);
    int width = lua_tointeger(context, 2);
    int height = lua_tointeger(context, 3);
    SDL_Window *result = gsdl_windowNew(title, width, height);
    list_add(glua_windows, (void*)result);
    return glua_windows->entries; // eventually, replace this with a windowID instead
}

lua_State *glua_initFunctions() {
    glua_windows = list_new(8);
    lua_State *globalState = luaL_newstate();
    luaL_openlibs(globalState);
    // Create the 'core' table and push it on the stack
        lua_newtable(globalState); 
        lua_setglobal(globalState, "core");
        lua_getglobal(globalState, "core");  
    // Register functions by pushing to the stack and then setting table fields as pointers
        lua_pushcfunction(globalState, glua_newWindow);  // Push the C function
        lua_setfield(globalState, -2, "newWindow");  // Set it as core.newWindow
    // Pop the 'core' table off the stack
        lua_pop(globalState, 1); 
    return globalState;
}