--- dofile("api.lua")

local window = gfx.newWindow("Test Window", 800, 600, gfx.flags.window.SHOWN)
local renderer = gfx.newRenderer(window, gfx.flags.renderer.ACCELERATED)
local texture = gfx.loadTexture(renderer, "./assets/ducks.jpg")
local font = gfx.loadFont("./assets/mono.ttf", 32)

--- @type integer
local frameLimit = 16

local box = function ( x, y, w, h ) return { x=x, y=y, w=w, h=h } end
local rgb = function ( r, g, b ) return { r, g, b, 255 } end
local rgba = function ( r, g, b, a ) return { r, g, b, a } end

local renderArea = { x = 32, y = 32, w = 256 + 32, h = 256 + 32 }
local textureBox = { x = 0, y = 0, w = 3569, h = 3569 }

gfx.setColor(renderer, 0, 0, 0, 255)
gfx.clear(renderer)

while true do
    gfx.clear(renderer)
    gfx.renderTexture(renderer, texture, textureBox, renderArea)
    gfx.presentRenderer(renderer)
    gfx.eventPoll()
    if ( gfx.event.type == gfx.flags.event.QUIT ) then  print("KILL SIGNAL RECEIVED") core.exit(0)
    elseif ( gfx.event.type == gfx.flags.event.WINDOWEVENT )
        then if ( gfx.event.event == gfx.flags.event.window.CLOSE ) then print("CLOSE SIGNAL RECEIVED") core.exit(0) end
    else gfx.delay(frameLimit)
    end
end