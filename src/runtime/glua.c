#ifndef MOONGIN 
    #include "includes.h"
#endif
void glua_error(lua_State *context, const char *fmt, const char *src, int arg) { sprintf(pushfmtstr, fmt, src, arg); lua_pushstring(context, pushfmtstr); lua_error(context); }

// ================ Get data on and off the stack

int glua_getInt(lua_State *context, char *func, int arg) {
    if (!lua_isnumber(context, -1)) {
        printf("ERR: type %s\n", lua_typename(context, lua_type(context, -1)));
        glua_error(context, sdls_expect_int, func, arg); 
        RET_ERR;
    }
    int value = lua_tointeger(context, -1);
    lua_pop(context, 1);  // Remove the top of the stack
    return value;
}

double glua_getFloat(lua_State *context, char *func, int arg) {
    if (!lua_isnumber(context, -1)) {
        glua_error(context, sdls_expect_float, func, arg);
        RET_ERR;
    }
    double value = lua_tonumber(context, -1);
    lua_pop(context, 1);
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

SDL_Rect glua_getBox(lua_State *context, char *func, int arg) {
    // Ensure the argument is a table
    SDL_Rect rect;
    if (!lua_istable(context, -1)) { glua_error(context, sdls_expect_table, func, 0); return BadBox; }
    lua_getfield(context, -1, "x");  // consume the value of "x"
        rect.x = glua_getInt(context, func, 1);
        if ( rect.x == ERROR ) { return BadBox; }
    lua_getfield(context, -1, "y"); // consume the 'y' value
        rect.y = glua_getInt(context, func, 2);
        if ( rect.y == ERROR ) { return BadBox; }
    lua_getfield(context, -1, "w"); // consume the 'w' value
        rect.w = glua_getInt(context, func, 3);
        if ( rect.w == ERROR ) { return BadBox; }
    lua_getfield(context, -1, "h"); // consume the 'h' value
        rect.h = glua_getInt(context, func, 4);
        if ( rect.h == ERROR ) { return BadBox; }
    lua_pop(context, 1); // pop off the table
    return rect;
}

// ===================== SDL Rendering Functions

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

int glua_exit(lua_State *context){
    int i; int status = glua_getInt(context, "core.exit", 1);
    int num_windows = glua_windows->entries;
    int num_renderers = glua_renderers->entries;
    for(i=0;i<num_windows;i++) { gsdl_windowDestroy( glua_windows->item[i] ); }
    for(i=0;i<num_renderers;i++) { gsdl_windowDestroy( glua_renderers->item[i] ); }
    gsdl_quit(); exit(status); 
}

int glua_newWindow(lua_State *context){
    int flags = glua_getInt(context, "gfx.newWindow", 4);
    int height = glua_getInt(context, "gfx.newWindow", 3);
    int width = glua_getInt(context, "gfx.newWindow", 2);
    const char *title = glua_getString(context, "gfx.newWindow", 1);
    if ( title == NULL | width == ERROR | height == ERROR ) { RET_ERR; }
    SDL_Window *result = gsdl_windowNew(title, width, height, flags);
    list_add(glua_windows, (void*)result);
    lua_pushinteger(context, item_newId(tag_windows, glua_windows->entries-1));
    return 1;
}

int glua_newRenderer(lua_State *context) {
    int flags = glua_getInt(context, "gfx.newRenderer", 2); if ( flags == ERROR ) { RET_ERR; }
    int index = glua_getInt(context, "gfx.newRenderer", 1); if ( index == ERROR ) { RET_ERR; }
    if ( item_isType(tag_windows, index) == false ) { glua_error(context, sdls_wrong_type, "gfx.newRenderer", 1); }
    SDL_Window *ref = item_byInt(index);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "gfx.newRenderer", 1); RET_ERR; }
    SDL_Renderer *result = gsdl_newRenderer(ref, flags);
    list_add(glua_renderers, (void*)result);
    lua_pushinteger(context, item_newId(tag_renderers, glua_renderers->entries-1));
    return 1;
}

int glua_presentRenderer(lua_State *context){
    int renderer_id = glua_getInt(context, "gfx.presentRenderer", 1); if ( renderer_id == ERROR ) { RET_ERR; }
    if ( item_isType(tag_renderers, renderer_id) == false) { glua_error(context, sdls_wrong_type, "gfx.presentRenderer", 1); RET_ERR; }
    SDL_Renderer *renderer = (SDL_Renderer*)item_byInt(renderer_id);
    if ( renderer == NULL ) { glua_error(context, sdls_invalid_id, "gfx.presentRenderer", 1); RET_ERR; }
    gsdl_presentRenderer(renderer);
    return OK;
}

int glua_destroyWindow(lua_State *context) {
    int id = glua_getInt(context, "gfx.destroyWindow", 1); if ( id == ERROR ) { RET_ERR; }
    if ( item_isType(tag_windows, id) == false ) { glua_error(context, sdls_wrong_type, "sdl_destroyWindow", 1); RET_ERR; }
    SDL_Window *ref = (SDL_Window*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "gfx.destroyWindow", 1); RET_ERR; }
    gsdl_windowDestroy(ref);
    return OK; // no arguments returned
}

int glua_destroyRenderer(lua_State *context) {
    int id = glua_getInt(context, "gfx.destroyRenderer", 1); if ( id == ERROR ) { RET_ERR; }
    if ( item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "sdl_destroyRenderer", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "gfx.destroyRenderer", 1); RET_ERR; }
    gsdl_rendererDestroy(ref);
    return OK;
}

int glua_loadTexture(lua_State *context) {
    const char *filename = glua_getString(context, "gfx.loadTexture", 2); if ( filename == NULL ) { RET_ERR; }
    int id = glua_getInt(context, "gfx.loadTexture", 1); if ( id == ERROR ) { RET_ERR; }
    if (item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "sdl_loadTexture", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "sdl_loadTexture", 1); RET_ERR; }
    SDL_Texture *result = gsdl_loadTexture(ref, filename);
    if ( result == NULL ) { glua_error(context, sdls_unable_file, "sdl_loadTexture", 2); RET_ERR; }
    list_add(glua_textures, (void*)result);
    lua_pushinteger(context, item_newId(tag_textures, glua_textures->entries-1));
    return 1;
}

int glua_renderTexture(lua_State *context) {
    // Get the output destination within the rendering context
        SDL_Rect dest = glua_getBox(context, "renderTexture", 4); if ( &dest == &BadBox ) { glua_error(context, sdls_expect_table_bad, "sdl_sdl_renderTexture", 4); RET_ERR; }
    // Get the bounding box for the texture
        SDL_Rect source = glua_getBox(context, "renderTexture", 3); if ( &source == &BadBox ) { glua_error(context, sdls_expect_table_bad, "sdl_sdl_renderTexture", 3); RET_ERR; }
    // Get the texture
    int texture_id = glua_getInt(context, "renderTexture", 2); if ( texture_id == ERROR ) { RET_ERR; }
        if (item_isType(tag_textures, texture_id) == false) { glua_error(context, sdls_wrong_type, "sdl_renderTexture", 2); RET_ERR; }
        SDL_Texture *texture = (SDL_Texture*)item_byInt(texture_id);
        if ( texture == NULL ) { glua_error(context, sdls_invalid_id, "sdl_renderTexture", 2); RET_ERR; }
    // Get the renderer
    int renderer_id = glua_getInt(context, "gfx.renderTexture", 1); if ( renderer_id == ERROR ) { RET_ERR; }
        if (item_isType(tag_renderers, renderer_id) == false) { glua_error(context, sdls_wrong_type, "sdl_renderTexture", 1); RET_ERR; }
        SDL_Renderer *renderer = (SDL_Renderer*)item_byInt(renderer_id);
        if ( renderer == NULL ) { glua_error(context, sdls_invalid_id, "sdl_renderTexture", 1); RET_ERR; }
    // Call out
        int result = gsdl_renderTexture(renderer, texture, source, dest);
        if ( result != OK ) { glua_error(context, sdls_unable_render, "renderTexture", 4); RET_ERR; }
        return OK;
}

int glua_setColor(lua_State *context) {
    int alpha = glua_getInt(context, "gfx.setColor", 4);   if ( alpha == ERROR ) { RET_ERR; }
    int blue = glua_getInt(context, "gfx.setColor", 3);    if ( blue == ERROR ) { RET_ERR; }
    int green = glua_getInt(context, "gfx.setColor", 2);   if ( green == ERROR ) { RET_ERR; }
    int red = glua_getInt(context, "gfx.setColor", 1);     if ( red == ERROR ) { RET_ERR; }
    int id = glua_getInt(context, "gfx.setColor", 1);      if ( id == ERROR ) { RET_ERR; }
    if (item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "gfx.setColor", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    if ( ref == NULL ) { glua_error(context, sdls_invalid_id, "gfx.setColor", 1); RET_ERR; }
    lua_pushinteger(context, gsdl_setColor(ref, red, green, blue, alpha));
    return 1;
}

int glua_clear(lua_State *context){
    int id = glua_getInt(context, "gfx.clear", 1); if ( id == ERROR ) { RET_ERR; }
    if (item_isType(tag_renderers, id) == false) { glua_error(context, sdls_wrong_type, "gfx.clear", 1); RET_ERR; }
    SDL_Renderer *ref = (SDL_Renderer*)item_byInt(id);
    gsdl_clear(ref);
    return OK;
}

int glua_delay(lua_State *context) {
    int ms = glua_getInt(context, "gfx.delay", 1); if ( ms == ERROR ) { RET_ERR; }
    gsdl_delay(ms);
    return OK;
}

// ================================ Stack Manipulation Functions

void glua_subfield_string(lua_State *state, const char *field, const char *value) {
    lua_pushstring(state, value);
    lua_setfield(state, -2, field);
}

void glua_subfield_int(lua_State *state, const char *field, int value) {
    lua_pushinteger(state, value);
    lua_setfield(state, -2, field);
}

void glua_enter_subtable(lua_State *state, const char *field) {
    lua_newtable(state);
    lua_setfield(state, -2, field);
    lua_getfield(state, -1, field);
}

// ========================== Entity System Functions

int glua_getByIdInt(lua_State *context) {
    int id = glua_getInt(context, "core.id", 1); if ( id == ERROR ) { RET_ERR; }
    void *reference = item_byInt(id);
    if ( reference == NULL ) { glua_error(context, sdls_invalid_id, "core.id", 1); }
    lua_pushlightuserdata(context, reference); // https://www.lua.org/pil/28.5.html
    return 1;
}

int glua_getByIdStr(lua_State *context) {
    const char *id = glua_getString(context, "core.parseid", 1); if ( id == NULL ) { RET_ERR; }
    void *reference = item_byStr(id);
    if ( reference == NULL ) { glua_error(context, sdls_invalid_id, "core.parseid", 1); }
    lua_pushlightuserdata(context, reference); // https://www.lua.org/pil/28.5.html
    return 1;
}

// ========================== Init

#define glua_initFunctionsDepth -2
void glua_method(lua_State *state, const char *name, lua_CFunction func) {
    lua_pushcfunction(state, func);
    lua_setfield(state, glua_initFunctionsDepth, name);
}

void glua_package_remove(lua_State *state, const char *package) {
    lua_pushnil(state); // Replace the metatable with a `nil` value
    lua_setglobal(state, package);
    lua_getglobal(state, "package"); // Also remove it from the packages table
    lua_getfield(state, -1, "loaded");
    lua_pushnil(state);
    lua_setfield(state, -2, "package");
    lua_pop(state, -1);
}

lua_State *glua_initFunctions() {
    // Push our data types to the global item table
    global_item_table = list_new(16);
    glua_windows = list_new(8); list_add(global_item_table, glua_windows);
    glua_renderers = list_new(8); list_add(global_item_table, glua_renderers);
    glua_textures = list_new(255); list_add(global_item_table, glua_textures);
    lua_State *globalState = luaL_newstate();
    luaL_openlibs(globalState);
    glua_package_remove(globalState, "os");
    // Remove 'os' from `package.loaded` too just in case anyone's trying to list the available packages
        lua_getglobal(globalState, "package");
        lua_getfield(globalState, -1, "loaded");
        lua_pushnil(globalState);
        lua_setfield(globalState, -2, "os");
    // Create empty lib tables
        lua_newtable(globalState);
        lua_setglobal(globalState, "core");
        lua_newtable(globalState);
        lua_setglobal(globalState, "gfx");
    // Set up CORE
        lua_getglobal(globalState, "core");
            glua_method(globalState, "exit", glua_exit);
            glua_method(globalState, "id", glua_getByIdInt);
            glua_method(globalState, "parseid", glua_getByIdStr);
        lua_pop(globalState, 1);
    // Set up GFX
        lua_getglobal(globalState, "gfx");
            glua_method(globalState, "newWindow",          glua_newWindow);
            glua_method(globalState, "destroyWindow",      glua_destroyWindow);
            glua_method(globalState, "newRenderer",        glua_newRenderer);
            glua_method(globalState, "destroyRenderer",    glua_destroyRenderer);
            glua_method(globalState, "presentRenderer",    glua_presentRenderer);
            glua_method(globalState, "loadTexture",        glua_loadTexture);
            glua_method(globalState, "renderTexture",      glua_renderTexture);
            glua_method(globalState, "setColor",           glua_setColor);
            glua_method(globalState, "clear",              glua_clear);
            glua_method(globalState, "delay",              glua_delay);
            glua_method(globalState, "eventPoll",          glua_pollEvent);
    // Events
        glua_enter_subtable(globalState, "event");
            glua_subfield_int(globalState, "type", -1);
            glua_subfield_int(globalState, "timestamp", -1);
            lua_pop(globalState, 1); // "events"
    // Flags    
        glua_enter_subtable(globalState, "flags");
            glua_enter_subtable(globalState, "window");
                glua_subfield_int(globalState, "FULLSCREEN",         0x00000001);
                glua_subfield_int(globalState, "OPENGL",             0x00000002);
                glua_subfield_int(globalState, "SHOWN",              0x00000004);
                glua_subfield_int(globalState, "HIDDEN",             0x00000008);
                glua_subfield_int(globalState, "BORDERLESS",         0x00000010);
                glua_subfield_int(globalState, "RESIZABLE",          0x00000020);
                glua_subfield_int(globalState, "MINIMIZED",          0x00000040);
                glua_subfield_int(globalState, "MAXIMIZED",          0x00000080);
                glua_subfield_int(globalState, "MOUSE_GRABBED",      0x00000100);
                glua_subfield_int(globalState, "INPUT_FOCUS",        0x00000200);
                glua_subfield_int(globalState, "MOUSE_FOCUS",        0x00000400);
                glua_subfield_int(globalState, "FULLSCREEN_DESKTOP", (0x00000001 | 0x00001000)); // FULLSCREEN | 0x00001000
                glua_subfield_int(globalState, "FOREIGN",            0x00000800);
                glua_subfield_int(globalState, "ALLOW_HIGHDPI",      0x00002000);
                glua_subfield_int(globalState, "MOUSE_CAPTURE",      0x00004000);
                glua_subfield_int(globalState, "ALWAYS_ON_TOP",      0x00008000);
                glua_subfield_int(globalState, "SKIP_TASKBAR",       0x00010000);
                glua_subfield_int(globalState, "UTILITY",            0x00020000);
                glua_subfield_int(globalState, "TOOLTIP",            0x00040000);
                glua_subfield_int(globalState, "POPUP_MENU",         0x00080000);
                glua_subfield_int(globalState, "KEYBOARD_GRABBED",   0x00100000);
                glua_subfield_int(globalState, "VULKAN",             0x10000000);
                glua_subfield_int(globalState, "METAL",              0x20000000);
                glua_subfield_int(globalState, "INPUT_GRABBED",      0x00000100);
                lua_pop(globalState, 1); // "window"
            glua_enter_subtable(globalState, "renderer");
                glua_subfield_int(globalState, "SOFTWARE",        0x00000001);
                glua_subfield_int(globalState, "ACCELERATED",     0x00000002);
                glua_subfield_int(globalState, "PRESENTVSYNC",    0x00000004);
                glua_subfield_int(globalState, "TARGETTEXTURE",   0x00000008);
                lua_pop(globalState, 1); // "renderer"
            glua_enter_subtable(globalState, "event");
                glua_subfield_int(globalState, "FIRSTEVENT", 0);
                glua_subfield_int(globalState, "QUIT", 0x100);
                glua_subfield_int(globalState, "APP_TERMINATING", 0x101);
                glua_subfield_int(globalState, "APP_LOWMEMORY", 0x102);
                glua_subfield_int(globalState, "APP_WILLENTERBACKGROUND", 0x103);
                glua_subfield_int(globalState, "APP_DIDENTERBACKGROUND", 0x104);
                glua_subfield_int(globalState, "APP_WILLENTERFOREGROUND", 0x105);
                glua_subfield_int(globalState, "APP_DIDENTERFOREGROUND", 0x106);
                glua_subfield_int(globalState, "LOCALECHANGED", 0x107);
                glua_subfield_int(globalState, "DISPLAYEVENT", 0x150);
                // Window Events
                glua_subfield_int(globalState, "WINDOWEVENT", 0x200);
                    glua_enter_subtable(globalState, "window");
                        glua_subfield_int(globalState, "NONE", 0);
                        glua_subfield_int(globalState, "SHOWN", 1);
                        glua_subfield_int(globalState, "HIDDEN", 2);
                        glua_subfield_int(globalState, "EXPOSED", 3);
                        glua_subfield_int(globalState, "MOVED", 4);
                        glua_subfield_int(globalState, "RESIZED", 5);
                        glua_subfield_int(globalState, "SIZE_CHANGED", 6);
                        glua_subfield_int(globalState, "MINIMIZED", 7);
                        glua_subfield_int(globalState, "MAXIMIZED", 8);
                        glua_subfield_int(globalState, "RESTORED", 9);
                        glua_subfield_int(globalState, "ENTER", 10);
                        glua_subfield_int(globalState, "LEAVE", 11);
                        glua_subfield_int(globalState, "FOCUS_GAINED", 12);
                        glua_subfield_int(globalState, "FOCUS_LOST", 13);
                        glua_subfield_int(globalState, "CLOSE", 14);
                        glua_subfield_int(globalState, "TAKE_FOCUS", 15);
                        glua_subfield_int(globalState, "HIT_TEST", 16);
                        glua_subfield_int(globalState, "ICCPROF_CHANGED", 17);
                        glua_subfield_int(globalState, "DISPLAY_CHANGED", 18);
                    lua_pop(globalState, 1); // "gfx.flags.event.window"
                glua_subfield_int(globalState, "SYSWMEVENT", 0x201);
                glua_subfield_int(globalState, "KEYDOWN", 0x300);
                glua_subfield_int(globalState, "KEYUP", 0x301);
                glua_subfield_int(globalState, "TEXTEDITING", 0x302);
                glua_subfield_int(globalState, "TEXTINPUT", 0x303);
                glua_subfield_int(globalState, "KEYMAPCHANGED", 0x304);
                glua_subfield_int(globalState, "TEXTEDITING_EXT", 0x305);
                glua_subfield_int(globalState, "MOUSEMOTION", 0x400);
                glua_subfield_int(globalState, "MOUSEBUTTONDOWN", 0x401);
                glua_subfield_int(globalState, "MOUSEBUTTONUP", 0x402);
                glua_subfield_int(globalState, "MOUSEWHEEL", 0x403);
                glua_subfield_int(globalState, "JOYAXISMOTION", 0x600);
                glua_subfield_int(globalState, "JOYBALLMOTION", 0x601);
                glua_subfield_int(globalState, "JOYHATMOTION", 0x602);
                glua_subfield_int(globalState, "JOYBUTTONDOWN", 0x603);
                glua_subfield_int(globalState, "JOYBUTTONUP", 0x604);
                glua_subfield_int(globalState, "JOYDEVICEADDED", 0x605);
                glua_subfield_int(globalState, "JOYDEVICEREMOVED", 0x606);
                glua_subfield_int(globalState, "JOYBATTERYUPDATED", 0x607);
                glua_subfield_int(globalState, "CONTROLLERAXISMOTION", 0x650);
                glua_subfield_int(globalState, "CONTROLLERBUTTONDOWN", 0x651);
                glua_subfield_int(globalState, "CONTROLLERBUTTONUP", 0x652);
                glua_subfield_int(globalState, "CONTROLLERDEVICEADDED", 0x653);
                glua_subfield_int(globalState, "CONTROLLERDEVICEREMOVED", 0x654);
                glua_subfield_int(globalState, "CONTROLLERDEVICEREMAPPED", 0x655);
                glua_subfield_int(globalState, "CONTROLLERTOUCHPADDOWN", 0x656);
                glua_subfield_int(globalState, "CONTROLLERTOUCHPADMOTION", 0x657);
                glua_subfield_int(globalState, "CONTROLLERTOUCHPADUP", 0x658);
                glua_subfield_int(globalState, "CONTROLLERSENSORUPDATE", 0x659);
                glua_subfield_int(globalState, "CONTROLLERUPDATECOMPLETE_RESERVED_FOR_SDL3", 0x65A);
                glua_subfield_int(globalState, "CONTROLLERSTEAMHANDLEUPDATED", 0x65B);
                glua_subfield_int(globalState, "FINGERDOWN", 0x700);
                glua_subfield_int(globalState, "FINGERUP", 0x701);
                glua_subfield_int(globalState, "FINGERMOTION", 0x702);
                glua_subfield_int(globalState, "DOLLARGESTURE", 0x800);
                glua_subfield_int(globalState, "DOLLARRECORD", 0x801);
                glua_subfield_int(globalState, "MULTIGESTURE", 0x802);
                glua_subfield_int(globalState, "CLIPBOARDUPDATE", 0x900);
                glua_subfield_int(globalState, "DROPFILE", 0x1000);
                glua_subfield_int(globalState, "DROPTEXT", 0x1001);
                glua_subfield_int(globalState, "DROPBEGIN", 0x1002);
                glua_subfield_int(globalState, "DROPCOMPLETE", 0x1003);
                glua_subfield_int(globalState, "AUDIODEVICEADDED", 0x1100);
                glua_subfield_int(globalState, "AUDIODEVICEREMOVED", 0x1101);
                glua_subfield_int(globalState, "SENSORUPDATE", 0x1200);
                glua_subfield_int(globalState, "RENDER_TARGETS_RESET", 0x2000);
                glua_subfield_int(globalState, "RENDER_DEVICE_RESET", 0x2001);
                glua_subfield_int(globalState, "POLLSENTINEL", 0x7F00);
                glua_subfield_int(globalState, "USEREVENT", 0x8000);
                glua_subfield_int(globalState, "LASTEVENT", 0xFFFF);
                lua_pop(globalState, 1); // "events"
            lua_pop(globalState, 1); // "flags"
        lua_pop(globalState, 1); // "gfx"
    return globalState;
}
