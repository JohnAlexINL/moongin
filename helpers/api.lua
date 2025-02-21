--- https://github.com/modula-dev/moongin
core = {}
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
--- @param width integer
--- @param height integer
--- @param flags integer [gfx.flags.window](gfx.flags.window)
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
--- @param red integer
--- @param green integer
--- @param blue integer
--- @param alpha integer
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