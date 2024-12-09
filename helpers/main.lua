-- dofile("api.lua")
local window = core.newWindow("Test Window", 800, 600) print( string.format("New window: \t%08x", window) ) 
local renderer = core.newRenderer(window, 0) print( string.format("New renderer: \t%08x", renderer) )
local texture = core.loadTexture(renderer, "./assets/duck.png") print( string.format("Duck texture: \t%08x", texture) )

local colors = {
    {255, 0, 0, 255},  -- Red
    {0, 255, 0, 255},  -- Green
    {0, 0, 255, 255},  -- Blue
    {255, 255, 0, 255},-- Yellow
    {0, 255, 255, 255},-- Cyan
    {255, 0, 255, 255},-- Magenta
    {0, 0, 0, 255},    -- Black
    {255, 255, 255, 255} -- White
}

-- Access and print each color's components
for i, color in ipairs(colors) do
    core.setColor(renderer, color[1], color[2], color[3], color[4])
    core.clear(renderer)
    core.delay(1000)
end
core.destroyRenderer(renderer)
core.destroyWindow(window)