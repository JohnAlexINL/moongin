#ifndef MOONGIN 
    #include "includes.h"
#endif
// ================================ The Event Router

int glua_pollEvent(lua_State *context) {
    gsdl_pollEvent();
    lua_getglobal(context, "gfx");
    lua_getfield(context, -1, "event");
    // Populate common attributes before returning
    glua_subfield_int(context, "type", eventHandle.common.type);
    glua_subfield_int(context, "timestamp", eventHandle.common.timestamp);
        switch( eventHandle.type ) {
        case SDL_POLLSENTINEL: break; // specifically nothing happened
        default: printf("_pollEvent: unhandled type %x\n", eventHandle.type); break;
        case SDL_WINDOWEVENT: printf("_pollEvent: Window Event %x sub %d\n", eventHandle.type, eventHandle.window.event); glua_event_window(context); break;
        case SDL_KEYDOWN: case SDL_KEYUP: printf("_pollEvent: Keyboard Event %x\n", eventHandle.type); glua_event_keyboard(context); break;
    }

    lua_pop(context, 2); // pop "gfx" and "event"
    return OK;
}

// ================================ Event Type Handlers

void glua_event_window(lua_State *context) {
    SDL_Window *window = SDL_GetWindowFromID(eventHandle.window.windowID);
    int window_id = list_index(glua_windows, window);
    glua_subfield_int(context, "window", window_id);
    glua_subfield_int(context, "event", eventHandle.window.event);
    glua_subfield_int(context, "data1", eventHandle.window.data1);
    glua_subfield_int(context, "data2", eventHandle.window.data2);
}

void glua_event_keyboard(lua_State *context) {
    SDL_Window *window = SDL_GetWindowFromID(eventHandle.key.windowID);
    int window_id = list_index(glua_windows, window);
    glua_subfield_int(context, "window", window_id);
    glua_subfield_int(context, "state", eventHandle.key.state);
    glua_subfield_int(context, "repeat", eventHandle.key.repeat);
    glua_subfield_int(context, "keycode", (eventHandle.key.keysym.sym));
    glua_subfield_int(context, "scancode", (eventHandle.key.keysym.scancode));
}
