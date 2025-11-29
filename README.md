### XVX

A lightweight & simple engine built with OpenGL and written in C, specifically
designed for simple games in mind. Born from curiosity how game engine works
under the hood.

### Why?

Why not?. This is fun **learning experience** for me. Now i knew a littebit how
engine works internally (mostly not). Don't expect perfect architecture, optimal
code or expect this engine have robust system, because it's not!. I'm learning
as I go, so there is some questionable decisions.

**Treat this a work-in-progress by someone who's still figuring things out!**

### What's Inside

- OpenGL rendering system
- Basic ECS - this what i know, don't expect pure ECS
- Cross-platform - (Linux & Windows)

### Dependencies

- **GLFW** - Window and input handling
- **bear** - Compilation database (i use this)

### Building

```bash
# Build compilation database (if you want)
bear -- make -f Makefile

# Build the project
make

# Run the testbed
./bin/xvx

```

### Current Status

Working prototype! Actively tested on Linux, with Windows support not tested yet.
The engine is currently tied directly to the testbed game, but will eventually becom a standalone library.
