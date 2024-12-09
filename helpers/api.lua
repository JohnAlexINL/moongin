core = {}
-- @param title string
-- @param width number
-- @param height number
core.newWindow = function(title, width, height) end
-- @param id number
-- @param flags number
core.newRenderer = function(id, flags) end
-- @param id number
-- @param filename string
core.loadTexture = function(id, filename) end
-- @param id number
core.destroyWindow = function(id) end
-- @param id number
core.destroyRenderer = function(id) end
-- @param id number
-- @param red number
-- @param green number
-- @param blue number
-- @param alpha number
core.setColor = function(id, red, green, blue, alpha) end
-- @param id number
core.clear = function(id) end
-- @param milliseconds number
core.delay = function(milliseconds) end