#ifndef MOONGIN 
    #include "includes.h"
#endif
// ================================ Glua Functions

int glua_byterun(lua_State *context, const char *bytecode, int bytecode_size);
int glua_exit(lua_State *context);
int glua_getInt(lua_State *context, char *func, int arg);
double glua_getFloat(lua_State *context, char *func, int arg);
const char *glua_getString(lua_State *context, char *func, int arg);
SDL_Rect glua_getBox(lua_State *context, char *func, int arg);
void glua_subfield_string(lua_State *state, const char *field, const char *value);
void glua_subfield_int(lua_State *state, const char *field, int value);
void glua_enter_subtable(lua_State *state, const char *field);

// ================================ SDL Functions

int glua_newWindow(lua_State *context);
int glua_newRenderer(lua_State *context);
int glua_presentRenderer(lua_State *context);
int glua_destroyWindow(lua_State *context);
int glua_destroyRenderer(lua_State *context);
int glua_loadTexture(lua_State *context);
int glua_renderTexture(lua_State *context);
int glua_setColor(lua_State *context);
int glua_clear(lua_State *context);
int glua_delay(lua_State *context);
int glua_pollEvent(lua_State *context);

// ================================ ENTITY SYSTEM

int glua_getByIdInt(lua_State *context);
int glua_getByIdStr(lua_State *context);

// ================================ MAGIC VALUES

list_t *glua_windows;       int tag_windows = 0;
list_t *glua_renderers;     int tag_renderers = 1;
list_t *glua_textures;      int tag_textures = 2; 

const char sdls_index_out_of_range[] = "%s (arg %d) index out of range";
const char sdls_index_destroyed[] = "%s (arg %d) cannot reuse destroyed instance";
const char sdls_expect_int[] = "%s (arg %d) expected integer";
const char sdls_expect_float[] = "%s (arg %d) expected floating-point number";
const char sdls_expect_table[] = "%s (arg %d) expected table";
const char sdls_expect_table_bad[] = "%s (arg %d) table missing required values";
const char sdls_expect_string[] = "%s (arg %d) expected string";
const char sdls_wrong_type[] = "%s (arg %d) wrong data type";
const char sdls_invalid_id[] = "%s (arg %d) given invalid id";
const char sdls_unable_file[] = "%s (arg %d) returned NULL, check that the file exists and is readable";
const char sdls_unable_render[] = "%s (arg %d) failed to render to canvas";
const char sdls_unable_handle[] = "%s (type %d) failed to process event";
