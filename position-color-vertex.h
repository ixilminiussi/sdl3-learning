#ifndef GRAPHICS_SDL3_POSITIONCOLORVERTEX
#define GRAPHICS_SDL3_POSITIONCOLORVERTEX

#include <SDL3/SDL_stdinc.h>

typedef struct PositionColorVertex
{
    float x, y, z;
    Uint8 r, g, b, a;
} PositionColorVertex;

#endif
