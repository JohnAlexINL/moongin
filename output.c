#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define OK 0
#define ERROR -1
// include "list.c"
typedef struct {
    int entries; int size;
    void **item;
} list_t;

void list_print(list_t *self){
    printf("list = { %d entries\n", self->entries);
    int i; for(i=0; i<self->entries; i++) {
        printf("    %d: %p\n", i, self->item[i]);
    }   printf("}\n");
}

list_t *list_new(int size){
    list_t *self = malloc(sizeof(list_t));
    self->entries = 0;
    self->size = size;
    self->item = malloc(sizeof(void*) * self->size);
    return self;
}

void list_add(list_t *self, void *item) {
    if (self->entries == self->size) {
        self->size *=2;
        self->item = realloc(self->item, sizeof(void*) * self->size);
    }   self->item[self->entries] = item;
    self->entries++;
}

void list_remove(list_t *self, int index) {
    int i; for(i=index; i<self->entries; i++) {
        self->item[i] = self->item[i+1];
    }
    if ( self->size < (self->entries/2) ) {
        self->size /=2;
        self->item = realloc(self->item, sizeof(void*) * self->size);
    }   self->entries--;
}

void list_concat(list_t *self, list_t *ref) {
    int i; for(i=0; i<ref->entries; i++) {
        list_add(self, ref->item[i]);
    }
}

bool list_find(list_t *self, void *item) {
    int i; for(i=0; i<self->entries; i++) {
        if (self->item[i] == item) { return true; }
    }   return false;
}
// include "item.c"
// TODO: engine item tags

list_t *global_item_table; // a list of lists, essentially

/*
    IDs look something like this:
    0x  xx aa aa aa
    where (x) is the type and (a) is the actual index;
    typeof 0 is reserved for errors only
*/
int item_newId(int type, int index) {
    int value = type + ( index << 8);
    return value;
}

bool item_isType(int type, int id) {
    int subtype = id % 255;
    if ( type == subtype) { return true; } return false;
}

void *item_byInt(int id) {
    int type = id % 255;
    int value = id >> 8;
    if ( global_item_table->entries <= type ) { printf("id-type outside of global types"); return NULL; }
    list_t *subTable = global_item_table->item[type];
    if ( subTable->entries <= value ) { printf("id-value outside defined values"); return NULL; }
    return subTable->item[value];
}

void *item_byStr(const char *id){
    if (strlen(id) != 8) { printf("ids should be 8 hexidecimal characters long"); return NULL; }
    int value; int i; for (i=0; i<8; i++) {
        if( id[i] <= 'f' && id[i] >= 'a' ){
            value *= 256;
            value += ( id[i] - 'a' + 10 );
        }
        else if ( id[i+2] <= '9' && id[i] >= '0' ) {
            value *= 256;
            value += ( id[i] - '0' );
        }
        else { printf("invalid character in id"); return NULL; }
    }   return item_byInt(value);
}
// include "gsdl.c"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int initialize_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return ERROR;
    }   return OK;
}

SDL_Window* gsdl_windowNew(const char *title, int width, int height) {
    SDL_Window *window = SDL_CreateWindow(
        title,                  // Title of the window
        SDL_WINDOWPOS_CENTERED, // X position of the window
        SDL_WINDOWPOS_CENTERED, // Y position of the window
        width,                  // Window width
        height,                 // Window height
        SDL_WINDOW_SHOWN        // Window flags (can include SDL_WINDOW_RESIZABLE, etc.)
    );
    if (window == NULL) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();  // Clean up SDL if window creation fails
        return NULL;  // Return NULL to indicate failure
    }   return window;
}

SDL_Renderer *gsdl_newRenderer(SDL_Window *parentWindow, int rendererFlags){
    SDL_Renderer *renderer = SDL_CreateRenderer(parentWindow, -1, rendererFlags);
    if (!renderer) { printf("SDL_CreateRenderer error: %s\n", SDL_GetError()); SDL_Quit(); return NULL; }
    return renderer;
}

void gsdl_windowDestroy(SDL_Window *window) {
    if (window) { SDL_DestroyWindow(window); }
}

void gsdl_rendererDestroy(SDL_Renderer *renderer) {
    if (renderer) { SDL_DestroyRenderer(renderer); }
}

bool gsdl_setColor(SDL_Renderer *renderer, char red, char green, char blue, char alpha) { return SDL_SetRenderDrawColor(renderer, red, green, blue, alpha); }
bool gsdl_clear(SDL_Renderer *renderer) { return SDL_RenderClear(renderer); }
bool gsdl_pollEvent(SDL_Event *event) { return SDL_PollEvent(event); }
void gsdl_delay(int milliseconds) { SDL_Delay(milliseconds); }

SDL_Texture *gsdl_loadTexture(SDL_Renderer *renderer, const char *filename) { 
    SDL_Texture *ref = IMG_LoadTexture(renderer, filename);
    if ( ref == NULL ) { printf("SDL_loadTexture error: %s\n", SDL_GetError()); return NULL; } // don't quit; we can recover from these types of errors
    return ref;
}
// include "glua.c"
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
// include "main.lua"
char lua_entry[] = {
    0x1b,0x4c,0x75,0x61,0x54,0x00,0x19,0x93,0x0d,0x0a,0x1a,0x0a,0x04,0x08,0x08,0x78,
    0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x28,0x77,0x40,0x01,
    0x94,0x40,0x2e,0x2f,0x68,0x65,0x6c,0x70,0x65,0x72,0x73,0x2f,0x6d,0x61,0x69,0x6e,
    0x2e,0x6c,0x75,0x61,0x80,0x80,0x00,0x01,0x06,0xa3,0x51,0x00,0x00,0x00,0x0b,0x00,
    0x00,0x00,0x0e,0x00,0x00,0x01,0x83,0x00,0x01,0x00,0x01,0x81,0x8f,0x81,0x81,0x81,
    0x2b,0x81,0x44,0x00,0x04,0x02,0x8b,0x00,0x00,0x03,0x0b,0x01,0x00,0x04,0x0e,0x01,
    0x02,0x05,0x83,0x01,0x03,0x00,0x00,0x02,0x00,0x00,0x44,0x01,0x03,0x00,0xc4,0x00,
    0x00,0x01,0x8b,0x00,0x00,0x00,0x8e,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x81,0x81,
    0xff,0x7f,0xc4,0x00,0x03,0x02,0x0b,0x01,0x00,0x03,0x8b,0x01,0x00,0x04,0x8e,0x01,
    0x03,0x05,0x03,0x02,0x04,0x00,0x80,0x02,0x01,0x00,0xc4,0x01,0x03,0x00,0x44,0x01,
    0x00,0x01,0x0b,0x01,0x00,0x00,0x0e,0x01,0x02,0x09,0x80,0x01,0x01,0x00,0x44,0x01,
    0x02,0x01,0x0b,0x01,0x00,0x00,0x0e,0x01,0x02,0x0a,0x80,0x01,0x00,0x00,0x44,0x01,
    0x02,0x01,0x46,0x01,0x01,0x01,0x8b,0x04,0x85,0x63,0x6f,0x72,0x65,0x04,0x8a,0x6e,
    0x65,0x77,0x57,0x69,0x6e,0x64,0x6f,0x77,0x04,0x8c,0x54,0x65,0x73,0x74,0x20,0x57,
    0x69,0x6e,0x64,0x6f,0x77,0x04,0x86,0x70,0x72,0x69,0x6e,0x74,0x04,0x87,0x73,0x74,
    0x72,0x69,0x6e,0x67,0x04,0x87,0x66,0x6f,0x72,0x6d,0x61,0x74,0x04,0x92,0x4e,0x65,
    0x77,0x20,0x77,0x69,0x6e,0x64,0x6f,0x77,0x3a,0x20,0x09,0x25,0x30,0x38,0x78,0x04,
    0x8c,0x6e,0x65,0x77,0x52,0x65,0x6e,0x64,0x65,0x72,0x65,0x72,0x04,0x94,0x4e,0x65,
    0x77,0x20,0x72,0x65,0x6e,0x64,0x65,0x72,0x65,0x72,0x3a,0x20,0x09,0x25,0x30,0x38,
    0x78,0x04,0x90,0x64,0x65,0x73,0x74,0x72,0x6f,0x79,0x52,0x65,0x6e,0x64,0x65,0x72,
    0x65,0x72,0x04,0x8e,0x64,0x65,0x73,0x74,0x72,0x6f,0x79,0x57,0x69,0x6e,0x64,0x6f,
    0x77,0x81,0x01,0x00,0x00,0x80,0xa3,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
    0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x80,0x82,0x87,0x77,0x69,0x6e,
    0x64,0x6f,0x77,0x87,0xa3,0x89,0x72,0x65,0x6e,0x64,0x65,0x72,0x65,0x72,0x93,0xa3,
    0x81,0x85,0x5f,0x45,0x4e,0x56,0x00
};


int main (int argc, char **argv) {
    lua_State *globalState = glua_initFunctions();
    int code = glua_byterun(globalState, lua_entry, sizeof(lua_entry));
}