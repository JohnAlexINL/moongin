--- https://github.com/modula-dev/moongin
core = {}
--- @param status number
core.exit = function(status) end
--- @class id
--- Returns a void pointer to the reference object
--- @param object id
--- @return lightuserdata pointer
core.id = function(object) end
--- Returns a void pointer to the reference object
--- @param object string id encoded as a hexidecimal string
--- @return lightuserdata pointer
core.parseid = function(object) end
--- https://wiki.libsdl.org/SDL2/FrontPage
gfx = { flags = {}}
--- @param title string Text to be displayed in titlebar; empty string is valid
--- @param width number
--- @param height number
--- @param flags number [gfx.flags.window](gfx.flags.window)
--- @return id window
gfx.newWindow = function(title, width, height, flags) end
--- @param window id
gfx.destroyWindow = function(window) end
--- @param window id
--- @param flags number [gfx.flags.renderer](gfx.flags.renderer)
--- @return id renderer
gfx.newRenderer = function(window, flags) end
--- Presents the content of the rendered frame to the screen
--- @param renderer id
gfx.presentRenderer = function(renderer) end
--- @param renderer id
gfx.destroyRenderer = function(renderer) end
--- @param renderer id
--- @param filename string
--- @return id texture
gfx.loadTexture = function(renderer, filename) end
--- @param renderer id
--- @param texture id
--- @param src box int: `x`, int: `y`, int: `w`, int: `h`
--- @param dest box int: `x`, int: `y`, int: `w`, int: `h`
gfx.renderTexture = function(renderer, texture, src, dest) end
--- @param red number
--- @param green number
--- @param blue number
--- @param alpha number
gfx.setColor = function(id, red, green, blue, alpha) end
--- @param renderer id
gfx.clear = function(renderer) end
--- @param milliseconds number
gfx.delay = function(milliseconds) end
--- https://wiki.libsdl.org/SDL2/SDL_WindowFlags
gfx.flags.window = {
    FULLSCREEN              = 0x00000001,
    OPENGL                  = 0x00000002,
    SHOWN                   = 0x00000004,
    HIDDEN                  = 0x00000008,
    BORDERLESS              = 0x00000010,
    RESIZABLE               = 0x00000020,
    MINIMIZED               = 0x00000040,
    MAXIMIZED               = 0x00000080,
    MOUSE_GRABBED           = 0x00000100,
    INPUT_FOCUS             = 0x00000200,
    MOUSE_FOCUS             = 0x00000400,
    FULLSCREEN_DESKTOP      = ( gfx.flags.window.FULLSCREEN | 0x00001000 ),
    FOREIGN                 = 0x00000800,            
    ALLOW_HIGHDPI           = 0x00002000,
    MOUSE_CAPTURE           = 0x00004000,
    ALWAYS_ON_TOP           = 0x00008000,
    SKIP_TASKBAR            = 0x00010000,
    UTILITY                 = 0x00020000,
    TOOLTIP                 = 0x00040000,
    POPUP_MENU              = 0x00080000,
    KEYBOARD_GRABBED        = 0x00100000,
    VULKAN                  = 0x10000000,
    METAL                   = 0x20000000,
    INPUT_GRABBED           = gfx.flags.window.MOUSE_GRABBED
}
--- https://wiki.libsdl.org/SDL2/SDL_RendererFlags
gfx.flags.renderer = {
    SOFTWARE                = 0x00000001,
    ACCELERATED             = 0x00000002,
    PRESENTVSYNC            = 0x00000004,
    TARGETTEXTURE           = 0x00000008
}
--- @class gfxEvent
--- @field type number
--- @field timestamp number
--- `eventpoll` takes and returns no arguments; `gfx.event` will be refreshed
--- https://wiki.libsdl.org/SDL2/SDL_Event
--- https://wiki.libsdl.org/SDL2/SDL_WindowEventID
gfx.eventPoll = function () end
gfx.flags.event = {
    FIRSTEVENT              = 0x0000,
    -- Application events
    QUIT                    = 0x0100,
    APP_TERMINATING         = 0x0101,
    APP_LOWMEMORY           = 0x0102,
    APP_WILLENTERBACKGROUND = 0x0103,
    APP_DIDENTERBACKGROUND  = 0x0104,
    APP_WILLENTERFOREGROUND = 0x0105,
    APP_DIDENTERFOREGROUND  = 0x0106,
    LOCALECHANGED           = 0x0107,
    -- Display events
    DISPLAYEVENT            = 0x0150,
    WINDOWEVENT             = 0x0200,
    SYSWMEVENT              = 0x0201,
    -- Keyboard events
    KEYDOWN                 = 0x0300,
    KEYUP                   = 0x0301,
    TEXTEDITING             = 0x0302,
    TEXTINPUT               = 0x0303,
    KEYMAPCHANGED           = 0x0304,
    TEXTEDITING_EXT         = 0x0305,
    -- Mouse events
    MOUSEMOTION             = 0x0400,
    MOUSEBUTTONDOWN         = 0x0401,
    MOUSEBUTTONUP           = 0x0402,
    MOUSEWHEEL              = 0x0403,
    -- Joystick events
    JOYAXISMOTION           = 0x0600,
    JOYBALLMOTION           = 0x0601,
    JOYHATMOTION            = 0x0602,
    JOYBUTTONDOWN          = 0x0603,
    JOYBUTTONUP            = 0x0604,
    JOYDEVICEADDED         = 0x0605,
    JOYDEVICEREMOVED       = 0x0606,
    JOYBATTERYUPDATED      = 0x0607,
    -- Game controller events
    CONTROLLERAXISMOTION      = 0x0650,
    CONTROLLERBUTTONDOWN      = 0x0651,
    CONTROLLERBUTTONUP        = 0x0652,
    CONTROLLERDEVICEADDED     = 0x0653,
    CONTROLLERDEVICEREMOVED   = 0x0654,
    CONTROLLERDEVICEREMAPPED  = 0x0655,
    CONTROLLERTOUCHPADDOWN    = 0x0656,
    CONTROLLERTOUCHPADMOTION  = 0x0657,
    CONTROLLERTOUCHPADUP      = 0x0658,
    CONTROLLERSENSORUPDATE    = 0x0659,
    CONTROLLERUPDATECOMPLETE_RESERVED_FOR_SDL3 = 0x065A,
    CONTROLLERSTEAMHANDLEUPDATED = 0x065B,
    -- Touch events
    FINGERDOWN              = 0x0700,
    FINGERUP                = 0x0701,
    FINGERMOTION            = 0x0702,
    -- Gesture events
    DOLLARGESTURE           = 0x0800,
    DOLLARRECORD            = 0x0801,
    MULTIGESTURE            = 0x0802,
    -- Clipboard events
    CLIPBOARDUPDATE         = 0x0900,
    -- Drag and drop events
    DROPFILE                = 0x1000,
    DROPTEXT                = 0x1001,
    DROPBEGIN               = 0x1002,
    DROPCOMPLETE            = 0x1003,
    -- Audio hotplug events
    AUDIODEVICEADDED        = 0x1100,
    AUDIODEVICEREMOVED      = 0x1101,
    -- Sensor events
    SENSORUPDATE            = 0x1200,
    -- Render events
    RENDER_TARGETS_RESET    = 0x2000,
    RENDER_DEVICE_RESET     = 0x2001,
    -- Internal events
    POLLSENTINEL            = 0x7F00,
    -- User events
    USEREVENT               = 0x8000,
    LASTEVENT               = 0xFFFF
}

gfx.flags.event.window = {
    NONE = 0,
    SHOWN = 1,
    HIDDEN = 2,
    EXPOSED = 3,
    MOVED = 4,
    RESIZED = 5,
    SIZE_CHANGED = 6,
    MINIMIZED = 7,
    MAXIMIZED = 8,
    RESTORED = 9,
    ENTER = 10,
    LEAVE = 11,
    FOCUS_GAINED = 12,
    FOCUS_LOST = 13,
    CLOSE = 14,
    TAKE_FOCUS = 15,
    HIT_TEST = 16,
    ICCPROF_CHANGED = 17,
    DISPLAY_CHANGED = 18
}