# Moongin

Moongin is a C+Lua game engine built against SDL2.
Eventually, the intent is to write a game engine
that can get built against `TinyGL`/`OpenGL` with
`SDL2` that can handle 3D Models and animations
created in Blender and be used for building
our games in an absolutely tiny and extensible engine.

## License

`moongin` is made available under the BSD 3-Clause License.
The code linked into executables produced with `moongin`
is shared code under the same license, and because of that
the permissive BSD license was chosen instead of something like GPLv3.

Basically, as long any program generated using Moongin 
includes an acknowledgement that it was built
with the following software, and does not imply endorsement, 
it is compliant with the license:

- `Moongin`, <https://modula.dev>
- `SDL`, <https://libsdl.org>
- `Lua`, <https://lua.org>

## Usage

`moongin` takes in a `.lua` file that functions as the
main logic of the program to build, and then embeds
it into a C-based runtime and compiles it with `clang`.
The output executable is **_not_** a static executable,
but I would like to eventually get to the point where
`moongin` does build static binaries.
It also expects the platforms to build for as additional flags.

## Moongin API

The full API is also annotated in the [`api.lua`](./helpers/api.lua) header

### `core` - engine functions

all entities store a copy of their `id`;
removing the `id` will cause erroneous behavior.
_TODO: if possible, make it so it can't be deleted
without calling the `destroy` method_

|--| Function | Interface | <abbr title="Source of Truth">sot</abbr> |
|--| -------- | ----------- | ---------------------------------------- |
|🗹| `core.parseid` | <abbr title="str: id">`str`</abbr> ➔ `void *` | _change to be `instance` instead_ |
|🗹| `core.id` | <abbr title="int: id">`int`</abbr> ➔ `void *` | _change to be `instance` instead_ |
|--| `core.castid` | <abbr title="int: type, void *: reference">`type, void *` ➔ <abbr title="int: id">`id`</abbr> | _this should be the only function that touches any `void *`; stored as `lightuserdata: ref`_ |
|--| `core.clone` | <abbr title="int: id, int: clonetype">`id, type`</abbr> ➔ `id` | |
|--| `core.type` | <abbr title="int: id">`id`</abbr> ➔ <abbr title="int: type">`type`</abbr> |  |
|--| `core.types` | _table_, _TODO:  Docs_ |  |
|--| `core.typedef` | `str` ➔ <abbr title="int: type">`int`</abbr> | |
|--| `core.typefunc` | `type, function` | |
|--| `core.destroy` | `int` | uses the type's destructor to free any data if exists, then deletes the entity instance | 
|🗹| `core.exit` | <abbr title="int: status">`int`</abbr> | |

#### types implemented by `core`

|--| Type | Field | Notes |
|--|--|--|--|
|--| `script` | _lua bytecode_ | _NOTE: `script` type cannot be cloned_|
|--| `room` | `baseid` | _if `baseid != id`_ then instanciated |
|--| | `init` | `id` script: takes `(room, table)`
|--| | `parent` | room `id`; root will have the window `id` |
|--| | `render` | draws contained objects and returns |
|--| | `pop` | returns to `parent` context unless at root and decrements `types.room.next` |
|--| | `sprite` | _optional_, _TODO: Docs_ |
|--| `object` | `baseid` | _if `baseid != id`_ then instanciated | 
|--| | `events` | pseudo `event.self(object, args)`; a named list of script `id`s
|--| | `init` | `id` script: takes `(object, table)`
|--| | `parent` | room `id` |
|--| | `sprite` | _TODO: Docs_, basically contains texture id, rect, and some optional fields |
|--| | `pop` | destroys the object and moves `type.self.next` back
### `gfx` - windowing, graphics, and i/o functions

|--| Function | Interface | <abbr title="Source of Truth">sot</abbr> |
|--| -------- | ----------- | ---------------------------------------- |
|🗹| `gfx.eventPoll` | modifies _gfx.event_ | [SDL_Event](https://wiki.libsdl.org/SDL2/SDL_Event) |
|--| `gfx.break` | <abbr title="table: callbacks, number: timeout">`table, timeout`</abbr> | _MOONGIN DOCS_ |
|🗹| `gfx.newWindow` | `title, width, height, flags` ➔ `id` | [SDL_CreateWindow](https://wiki.libsdl.org/SDL2/SDL_CreateWindow) |
|🗹| `gfx.destroyWindow` | `id` | [SDL_DestroyWindow](https://wiki.libsdl.org/SDL2/SDL_DestroyWindow) |
|🗹| `gfx.newRenderer` | `window, flags` ➔ `id` | [SDL_CreateRenderer](https://wiki.libsdl.org/SDL2/SDL_CreateRenderer) |
|🗹| `gfx.destroyRenderer` | `id` | [SDL_DestroyRenderer](https://wiki.libsdl.org/SDL2/SDL_DestroyRenderer) |
|🗹| `gfx.presentRenderer` | `id` | [SDL_RenderPresent](https://wiki.libsdl.org/SDL2/SDL_RenderPresent) |
|--| `gfx.getRenderViewport` | `id` ➔ `box` | [SDL_RenderGetViewport](https://wiki.libsdl.org/SDL2/SDL_RenderGetViewport) |
|🗹| `gfx.setColor` | <abbr title="int: red, int: green, int: blue, int: alpha">`rgba` | [SDL_SetRenderDrawColor](https://wiki.libsdl.org/SDL2/SDL_SetRenderDrawColor) |
|--| `gfx.setBlendMode` | `flags` | [SDL_BlendMode](https://wiki.libsdl.org/SDL2/SDL_BlendMode)
|🗹| `gfx.clear` | <abbr title="id: renderer">`id`</abbr> | [SDL_RenderClear](https://wiki.libsdl.org/SDL2/SDL_RenderClear), [SDL_RenderPresent](https://wiki.libsdl.org/SDL2/SDL_RenderPresent) |
|--| `gfx.renderDot` | `id, point` | [CategoryRender](https://wiki.libsdl.org/SDL2/CategoryRender) |
|--| `gfx.renderDots` | `id, list` | _list of points_  |
|--| `gfx.renderLine` | `id, point, point` | |
|--| `gfx.renderLines` | `id, list` | _list of points_ |
|--| `gfx.renderLineBox` | `id, box` | _no fill_ |
|--| `gfx.renderLineBoxes` | `id, list` | _no fill, list of boxes_ |
|--| `gfx.renderBox` | `id, box` | |
|--| `gfx.renderBoxes` | `id, list` | _list of boxes_ |
|🗹| `gfx.delay` | <abbr title="number: number of milliseconds">`number`</abbr> | [SDL_Delay](https://wiki.libsdl.org/SDL2/SDL_Delay) |
|🗹| `gfx.loadTexture` | <abbr title="id: renderer, filename: texture">`id, filename`</abbr> ➔ `id` | [IMG_LoadTexture](https://wiki.libsdl.org/SDL2_image/IMG_LoadTexture) |
|🗹| `gfx.renderTexture` | <abbr title="id: renderer, id: texture, box: texture area, box: renderer location">`id, id, box, box`</abbr> | [SDL_RenderCopy](https://wiki.libsdl.org/SDL2/SDL_RenderCopy) |
|--| `gfx.renderRotatedTexture` <abbr title="id: renderer, id: texture, box: texture area, box: renderer location, number: angle, int: flags">`id, id, box, box, angle, flags`</abbr> | [SDL_RenderCopyEx](https://wiki.libsdl.org/SDL2/SDL_RenderCopyEx) |
|--| `gfx.colorTexture` | `id, rgb` | [SDL_SetTextureColorMod](https://wiki.libsdl.org/SDL2/SDL_SetTextureColorMod) |
|--| `gfx.alphaTexture` | `id, alpha` | [SDL_SetTextureAlphaMod](https://wiki.libsdl.org/SDL2/SDL_SetTextureAlphaMod) |
|--| `gfx.blendModeTexture` | `id, flags` | [SDL_BlendMode](https://wiki.libsdl.org/SDL2/SDL_BlendMode) |
|--| `gfx.scaleModeTexture` | `id, flags` | [SDL_SetTextureScaleMode](https://wiki.libsdl.org/SDL2/SDL_SetTextureScaleMode)
|--| `gfx.readRenderer` | `id, box` ➔ <abbr title="list: rgba values">`list`</abbr> | [SDL_RenderReadPixels](https://wiki.libsdl.org/SDL2/SDL_RenderReadPixels) |
|--| `gfx.loadFont` | `filepath, ptsize` ➔ `id`  | [TTF_OpenFont](https://wiki.libsdl.org/SDL2_ttf/TTF_OpenFont) |
|--| `gfx.destroyFont` | `id` | [TTF_CloseFont](https://wiki.libsdl.org/SDL2_ttf/TTF_CloseFont) |
|--| `gfx.measureFont` | `id, text, width` ➔ `chars` | *Return the number of characters* [TTF_MeasureUTF8](https://wiki.libsdl.org/SDL2_ttf/TTF_MeasureUTF8) |
|--| `gfx.resizeFont` | `id, ptsize` | [TTF_SetFontSize](https://wiki.libsdl.org/SDL2_ttf/TTF_SetFontSize) |
|--| `gfx.getStyleFont` | `id` ➔ `flags` | [TTF_GetFontStyle](https://wiki.libsdl.org/SDL2_ttf/TTF_GetFontStyle) |
|--| `gfx.setStyleFont` | `id, flags` | [TTF_SetFontStyle](https://wiki.libsdl.org/SDL2_ttf/TTF_SetFontStyle) |
|--| `gfx.renderFontSolid` | `id, string` | [TTF_RenderUNICODE_Solid](https://wiki.libsdl.org/SDL2_ttf/TTF_RenderUTF8_Solid) |
|--| `gfx.renderFontBlended` | `id, string` | [TTF_RenderUNICODE_Blended](https://wiki.libsdl.org/SDL2_ttf/TTF_RenderUTF8_Blended) |

<!-- 
    SDL_SetTextureUserData ? direct manipulating of pixels
    SDL_LockTexture + SDL_UnlockTexture ? possibly needed for texture i/o operations
    SDL_RenderGeometry maybe for adding 3D/hand-rendering
-->