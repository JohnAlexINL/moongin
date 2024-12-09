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
    if (!lua_isstring(context, 1))  { lua_pushstring(context, "sdl_newWindow expected string title for argument 1"); lua_error(context); return ERROR; }
    if (!lua_isnumber(context, 2)) { lua_pushstring(context, "sdl_newWindow expected int width for argument 1"); lua_error(context); return ERROR; }
    if (!lua_isnumber(context, 3)) { lua_pushstring(context, "sdl_newWindow expected int height for argument 2"); lua_error(context); return ERROR; }
    const char *title = lua_tostring(context, 1);
    int width = lua_tointeger(context, 2);
    int height = lua_tointeger(context, 3);
    SDL_Window *result = gsdl_windowNew(title, width, height);
    list_add(glua_windows, (void*)result);
    int id = glua_windows->entries-1; // eventually, replace this with a windowID instead
    lua_pushinteger(context, id);
    return 1; // return # of args returned to stack
}

int glua_destroyWindow(lua_State *context) {
    if (!lua_isinteger(context, 1))  { lua_pushstring(context, "sdl_destroyWindow expected the window index"); lua_error(context); return ERROR; }
    int index = lua_tointeger(context, 1);
    if (index >= glua_windows->entries) { lua_pushstring(context, "sdl_destroyWindow given index out of range"); lua_error(context); return ERROR; }
    if (glua_windows->item[index] == NULL) { lua_pushstring(context, "sdl_destroyWindow given already destroyed window"); lua_error(context); return ERROR; }
    SDL_Window *ref = glua_windows->item[index];
    gsdl_windowDestroy(ref);
    return OK; // no arguments returned
}

lua_State *glua_initFunctions() {
    glua_windows = list_new(8);
    lua_State *globalState = luaL_newstate();
    luaL_openlibs(globalState);
    // Disable the 'os' library by setting it to nil
        lua_pushnil(globalState);
        lua_setglobal(globalState, "os");
    // Remove 'os' from `package.loaded` too just in case anyone's trying to list the available packages
        lua_getglobal(globalState, "package");
        lua_getfield(globalState, -1, "loaded");
        lua_pushnil(globalState);
        lua_setfield(globalState, -2, "os");
    // Create the 'core' table and push it on the stack
        lua_newtable(globalState); 
        lua_setglobal(globalState, "core");
        lua_getglobal(globalState, "core");
    // Register functions by pushing to the stack and then setting table fields as pointers
        lua_pushcfunction(globalState, glua_newWindow);  // Push the C function
        lua_setfield(globalState, -2, "newWindow");  // Set it as core.newWindow
        lua_pushcfunction(globalState, glua_destroyWindow);  // Push the C function
        lua_setfield(globalState, -2, "destroyWindow");  // Set it as core.newWindow
    // Pop the 'core' table off the stack
        lua_pop(globalState, 1); 
    return globalState;
}