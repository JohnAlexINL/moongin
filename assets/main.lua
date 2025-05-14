--- dofile("api.lua")

local window = gfx.newWindow("Test Window", 800, 600, gfx.flags.window.SHOWN) print( string.format("New window: \t%08x", window) ) 
local renderer = gfx.newRenderer(window, 0) print( string.format("New renderer: \t%08x", renderer) )
local texture = gfx.loadTexture(renderer, "./assets/duck.png") print( string.format("Duck texture: \t%08x", texture) )

--- @type integer
local frameLimit = 30

local box = function ( x, y, w, h ) return { x=x, y=y, w=w, h=h } end
local rgb = function ( r, g, b ) return { r, g, b, 255 } end
local rgba = function ( r, g, b, a ) return { r, g, b, a } end

local colors = {
    rgb(255,0,0),
    rgb(0,255,0),
    rgb(0,0,255),
    rgb(255,255,0),
    rgb(0,255,255),
    rgb(255,0,255),
    rgb(255,255,255),
    rgb(0,0,0)
}

local renderArea = { x = 32, y = 32, w = 255, h = 255 }
local textureBox = { x = 0, y = 0, w = 1508, h = 1770 }

gfx.setColor(renderer, 0, 0, 0, 255)
gfx.clear(renderer)

print( "waiting for a QUIT event, which looks like >> " )
print( gfx.flags.event.QUIT )

for i = 1, 300 do
    for i, color in ipairs(colors) do
        gfx.setColor(renderer, color[1], color[2], color[3], color[4])
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
end
print( string.format("entity id %08x, pointer to *%p", renderer, core.id(renderer)) )
print( string.format("entity id \"%s\", pointer to *%p", tostring(renderer), core.id(renderer)) )
gfx.destroyRenderer(renderer)
gfx.destroyWindow(window)