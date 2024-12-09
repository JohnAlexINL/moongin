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
// include "gsdl.c"
#include <stdio.h>
#include <SDL2/SDL.h>

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

void gsdl_windowClean(SDL_Window *window) {
    if (window) { SDL_DestroyWindow(window); }
}
// include "glua.c"
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
// include "main.lua"
char lua_entry[] = {
    0x1b,0x4c,0x75,0x61,0x54,0x00,0x19,0x93,0x0d,0x0a,0x1a,0x0a,0x04,0x08,0x08,0x78,
    0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x28,0x77,0x40,0x01,
    0x94,0x40,0x2e,0x2f,0x68,0x65,0x6c,0x70,0x65,0x72,0x73,0x2f,0x6d,0x61,0x69,0x6e,
    0x2e,0x6c,0x75,0x61,0x80,0x80,0x00,0x01,0x04,0x8c,0x51,0x00,0x00,0x00,0x0b,0x00,
    0x00,0x00,0x0e,0x00,0x00,0x01,0x83,0x00,0x01,0x00,0x01,0x81,0x8f,0x81,0x81,0x81,
    0x2b,0x81,0x44,0x00,0x04,0x01,0x0b,0x00,0x00,0x03,0x0e,0x00,0x00,0x04,0x83,0x80,
    0x02,0x00,0x44,0x00,0x02,0x01,0x46,0x00,0x01,0x01,0x86,0x04,0x85,0x63,0x6f,0x72,
    0x65,0x04,0x8a,0x6e,0x65,0x77,0x57,0x69,0x6e,0x64,0x6f,0x77,0x04,0x8c,0x54,0x65,
    0x73,0x74,0x20,0x57,0x69,0x6e,0x64,0x6f,0x77,0x04,0x83,0x6f,0x73,0x04,0x88,0x65,
    0x78,0x65,0x63,0x75,0x74,0x65,0x04,0x88,0x73,0x6c,0x65,0x65,0x70,0x20,0x33,0x81,
    0x01,0x00,0x00,0x80,0x8c,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
    0x00,0x80,0x80,0x81,0x85,0x5f,0x45,0x4e,0x56,0x00
};


int main (int argc, char **argv) {
    lua_State *globalState = glua_initFunctions();
    int code = glua_byterun(globalState, lua_entry, sizeof(lua_entry));
}