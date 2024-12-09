#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>

const char sdls_index_out_of_range[] = "%s (arg %d) index out of range";
const char sdls_index_destroyed[] = "%s (arg %d) cannot reuse destroyed instance";
const char sdls_expect_int[] = "%s (arg %d) expected integer";
const char sdls_expect_string[] = "%s (arg %d) expected string";
const char sdls_wrong_type[] = "%s (arg %d) wrong data type";
const char sdls_invalid_id[] = "%s (arg %d) given invalid id";
const char sdls_unable_file[] = "%s (arg %d) returned NULL, check that the file exists and is readable";

char pushfmtstr[255];
#define RET_ERR return 1
void glua_error(lua_State *context, const char *fmt, const char *src, int arg) { sprintf(pushfmtstr, fmt, src, arg); lua_pushstring(context, pushfmtstr); lua_error(context); }

int glua_getInt(lua_State *context, char *func, int arg) {
    if (!lua_isinteger(context, -1)) {
        glua_error(context, sdls_expect_int, func, arg); 
        RET_ERR;
    }
    int value = lua_tointeger(context, -1);
    lua_pop(context, 1);  // Remove the top of the stack
    return value;
}

const char *glua_getString(lua_State *context, char *func, int arg) {
    if (!lua_isstring(context, -1)) {
        glua_error(context, sdls_expect_string, func, arg);
        return NULL;
    }
    const char *value = lua_tostring(context, -1);
    lua_pop(context, 1);  // Remove the top of the stack
    return value;
}

list_t *glua_windows;       int tag_windows = 0;
list_t *glua_renderers;     int tag_renderers = 1;
list_t *glua_textures;      int tag_textures = 2;

int glua_byterun(lua_State *context, const char *bytecode, int bytecode_size) {
    if (luaL_loadbuffer(context, bytecode, bytecode_size, "chunk") != LUA_OK) {
        printf("Error loading Lua module: %s\n", lua_tostring(context, -1));
        return ERROR;
    }
    if (lua_pcall(context, 0, LUA_MULTRET, 0) != LUA_OK) {
        printf("Error in Lua runtime: %s\n", lua_tostring(context, -1));
        return ERROR;
    }
    return OK;
}

int glua_newWindow(lua_State *context){
    int height = glua_getInt(context, "core.newWindow", 3);
    int width = glua_getInt(context, "core.newWindow", 2);
    const char *title = glua_getString(context, "core.newWindow", 1);
    if ( title == NULL | width == ERROR | height == ERROR ) { RET_ERR; }
    SDL_Window *result = gsdl_windowNew(title, width, height);
    list_add(glua_windows, (void*)result);
    lua_pushinteger(context, item_newId(tag_windows, glua_windows->entries-1));
    return 1;
}

int glua_newRenderer(lua_State *context) {
    int flags = glua_getInt(context, "core.newWindow", 2); if ( flags == ERROR ) { RET_ERR; }
    int index = glua_getInt(context, "core.newWindow", 1); if ( index == ERROR ) { RET_ERR; }
    if ( item_isType(tag_windows, index) == false ) { glua_error(context, sdls_wrong_type, "sdl_newRenderer", 1); }
    SDL_Window *ref = item_byInt(index);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "sdl_newRenderer", 1); RET_ERR; }
    SDL_Renderer *result = gsdl_newRenderer(ref, flags);
    list_add(glua_renderers, (void*)result);
    lua_pushinteger(context, item_newId(tag_renderers, glua_renderers->entries-1));
    return 1;
}

int glua_destroyWindow(lua_State *context) {
    int id = glua_getInt(context, "core.destroyWindow", 1); if ( id == ERROR ) { RET_ERR; }
    if ( item_isType(tag_windows, id) == false ) { glua_error(context, sdls_wrong_type, "sdl_destroyWindow", 1); RET_ERR; }
    SDL_Window *ref = (SDL_Window*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "sdl_destroyWindow", 1); RET_ERR; }
    gsdl_windowDestroy(ref);
    return OK; // no arguments returned
}

int glua_destroyRenderer(lua_State *context) {
    int id = glua_getInt(context, "core.destroyRenderer", 1); if ( id == ERROR ) { RET_ERR; }
    if ( item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "sdl_destroyRenderer", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "core.destroyRenderer", 1); RET_ERR; }
    gsdl_rendererDestroy(ref);
    return OK;
}

int glua_loadTexture(lua_State *context) {
    const char *filename = glua_getString(context, "core.loadTexture", 2); if ( filename == NULL ) { RET_ERR; }
    int id = glua_getInt(context, "core.loadTexture", 1); if ( id == ERROR ) { RET_ERR; }
    if (item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "sdl_loadTexture", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "sdl_loadTexture", 1); RET_ERR; }
    SDL_Texture *result = gsdl_loadTexture(ref, filename);
    if ( result == NULL ) { glua_error(context, sdls_unable_file, "sdl_loadTexture", 2); RET_ERR; }
    list_add(glua_textures, (void*)result);
    lua_pushinteger(context, item_newId(tag_textures, glua_textures->entries-1));
    return 1;
}

int glua_setColor(lua_State *context) {
    int alpha = glua_getInt(context, "core.setColor", 4);   if ( alpha == ERROR ) { RET_ERR; }
    int blue = glua_getInt(context, "core.setColor", 3);    if ( blue == ERROR ) { RET_ERR; }
    int green = glua_getInt(context, "core.setColor", 2);   if ( green == ERROR ) { RET_ERR; }
    int red = glua_getInt(context, "core.setColor", 1);     if ( red == ERROR ) { RET_ERR; }
    int id = glua_getInt(context, "core.setColor", 1);      if ( id == ERROR ) { RET_ERR; }
    if (item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "core.setColor", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "core.setColor", 1); RET_ERR; }
    lua_pushinteger(context, gsdl_setColor(ref, red, green, blue, alpha));
    return 1;
}

int glua_clear(lua_State *context){
    int id = glua_getInt(context, "core.clear", 1); if ( id == ERROR ) { RET_ERR; }
    if (item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "core.clear", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    gsdl_clear(ref);
    return OK;
}

int glua_delay(lua_State *context) {
    int ms = glua_getInt(context, "core.delay", 1); if ( ms == ERROR ) { RET_ERR; }
    gsdl_delay(ms);
    return OK;
}

// TODO: pollEvent

lua_State *glua_initFunctions() {
    // Push our data types to the global item table
    global_item_table = list_new(16);
    glua_windows = list_new(8); list_add(global_item_table, glua_windows);
    glua_renderers = list_new(8); list_add(global_item_table, glua_renderers);
    glua_textures = list_new(255); list_add(global_item_table, glua_textures);
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
        lua_pushcfunction(globalState, glua_newWindow);
        lua_setfield(globalState, -2, "newWindow");
        lua_pushcfunction(globalState, glua_destroyWindow);
        lua_setfield(globalState, -2, "destroyWindow");
        lua_pushcfunction(globalState, glua_newRenderer);
        lua_setfield(globalState, -2, "newRenderer");
        lua_pushcfunction(globalState, glua_destroyRenderer);
        lua_setfield(globalState, -2, "destroyRenderer");
        lua_pushcfunction(globalState, glua_loadTexture);
        lua_setfield(globalState, -2, "loadTexture");
        lua_pushcfunction(globalState, glua_setColor);
        lua_setfield(globalState, -2, "setColor");
        lua_pushcfunction(globalState, glua_clear);
        lua_setfield(globalState, -2, "clear");
        lua_pushcfunction(globalState, glua_delay);
        lua_setfield(globalState, -2, "delay");
    // Pop the 'core' table off the stack
        lua_pop(globalState, 1); 
    return globalState;
}