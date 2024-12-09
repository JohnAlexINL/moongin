-- dofile("api.lua")
local i = core.newWindow("Test Window", 800, 600)
print("New window = ", i)
core.destroyWindow(i)