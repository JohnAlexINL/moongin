#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>

const char sdls_index_out_of_range[] = "%s index out of range";
const char sdls_index_destroyed[] = "%s cannot reuse destroyed instance";
const char sdls_expect_int_1[] = "%s expected integer as argument 1";
const char sdls_expect_int_2[] = "%s expected integer as argument 2";
const char sdls_expect_int_3[] = "%s expected integer as argument 3";
const char sdls_expect_string_1[] = "%s expected string as argument 1";
const char sdls_expect_string_2[] = "%s expected string as argument 2";
const char sdls_expect_string_3[] = "%s expected string as argument 3";
const char sdls_wrong_type_1[] = "%s given id for wrong data type";
const char sdls_wrong_type_2[] = "%s given id for wrong data type";
const char sdls_wrong_type_3[] = "%s given id for wrong data type";
const char sdls_invalid_id[] = "%s given invalid id";

char pushfmtstr[255];
void glua_error(lua_State *context, const char *fmt, char *src){ sprintf(pushfmtstr, fmt, src); lua_pushstring(context, pushfmtstr); lua_error(context); }

list_t *glua_windows;       int tag_windows = 0;
list_t *glua_renderers;     int tag_renderers = 1;
list_t *glua_textures;      int tag_textures = 2;

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
    if (!lua_isstring(context, 1))  { glua_error(context, sdls_expect_string_1, "sdl_newWindow"); return ERROR; }
    if (!lua_isnumber(context, 2)) { glua_error(context, sdls_expect_int_2, "sdl_newWindow"); return ERROR; }
    if (!lua_isnumber(context, 3)) { glua_error(context, sdls_expect_int_3, "sdl_newWindow"); return ERROR; }
    const char *title = lua_tostring(context, 1);
    int width = lua_tointeger(context, 2);
    int height = lua_tointeger(context, 3);
    SDL_Window *result = gsdl_windowNew(title, width, height);
    list_add(glua_windows, (void*)result);
    int id = item_newId(tag_windows, glua_windows->entries-1); // eventually, replace this with a windowID instead
    lua_pushinteger(context, id);
    return 1; // return # of args returned to stack
}

int glua_newRenderer(lua_State *context) {
    if (!lua_isnumber(context, 1)) { glua_error(context, sdls_expect_int_1, "sdl_newRenderer"); return ERROR; }
    if (!lua_isnumber(context, 2)) { glua_error(context, sdls_expect_int_2, "sdl_newRenderer"); return ERROR; }
    int index = lua_tointeger(context, 1);
    int flags = lua_tointeger(context, 2);
    if ( item_isType(tag_windows, index) == false ) { glua_error(context, sdls_wrong_type_1, "sdl_newRenderer"); return ERROR; }
    SDL_Window *ref = item_byInt(index);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "sdl_newRenderer"); return ERROR; }
    SDL_Renderer *result = gsdl_newRenderer(ref, flags);
    list_add(glua_renderers, (void*)result);
    int id = item_newId(tag_renderers, glua_renderers->entries-1);
    lua_pushinteger(context, id);
    return 1; // return # of args returned to stack
}

int glua_destroyWindow(lua_State *context) {
    if (!lua_isinteger(context, 1))  { glua_error(context, sdls_expect_int_1, "sdl_destroyWindow"); return ERROR; }
    int id = lua_tointeger(context, 1);
    if ( item_isType(tag_windows, id) == false ) { glua_error(context, sdls_wrong_type_1, "sdl_destroyWindow"); return ERROR; }
    SDL_Window *ref = (SDL_Window*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "sdl_destroyWindow"); return ERROR; }
    gsdl_windowDestroy(ref);
    return OK; // no arguments returned
}

int glua_destroyRenderer(lua_State *context) {
    if (!lua_isinteger(context, 1)) { glua_error(context, sdls_expect_int_1, "sdl_destroyRenderer"); return ERROR; }
    int id = lua_tointeger(context, 1);
    if ( item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type_1, "sdl_destroyRenderer"); return ERROR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "sdl_destroyRenderer"); return ERROR; }
    gsdl_rendererDestroy(ref);
    return OK;
}

// TODO: setColor, clear, pollEvent, delay

int glua_loadTexture(lua_State *context) {
    if (!lua_isinteger(context, 1))  { glua_error(context, sdls_expect_int_1, "sdl_loadTexture"); return ERROR; }
    if (!lua_isstring(context, 2))  { glua_error(context, sdls_expect_string_2, "sdl_loadTexture"); return ERROR; }
    int index = lua_tointeger(context, 1);
    if (index >= glua_windows->entries) { glua_error(context, sdls_index_out_of_range, "sdl_loadTexture"); return ERROR; }
    if (glua_windows->item[index] == NULL) { glua_error(context, sdls_index_destroyed, "sdl_loadTexture"); return ERROR; }
    SDL_Window *ref = glua_windows->item[index];
    const char *filename = lua_tostring(context, 2);
    return 1; // TODO: return texture id
}

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
    // Pop the 'core' table off the stack
        lua_pop(globalState, 1); 
    return globalState;
}