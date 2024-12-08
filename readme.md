# Moongin

is the Modula game engine.
It will be using Lua as the core language,
and will be built primarily against SDL2.
I had wanted to try SDL3 but just couldn't
get it building right, and it doesn't seem
mature enough that there's really any
"help" getting it yet, so.

## Architecture

`moongin.brew` takes in a `*.lua` file to bake in as the main function
and then writes a `main.c` and configures some compiler variables
to produce a binary for Windows, Linux, and Mac

`moongin.c` is the main include needed for building a program
with the moon engine, and will be included by the generated `main.c`,
and includes stuff like generic data structures,
the engine's API, the main loop, et cetera

`console.c` is a tiny debugging console that allows typing in
Lua commands that will be immediately executed by the engine

`glua.c` contains glue functions for connecting Lua functions to the engine

`gsdl.c` contains glue functions for SDL2 to the engine