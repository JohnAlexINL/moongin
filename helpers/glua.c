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