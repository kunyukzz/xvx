#ifndef LIGHTING_H
#define LIGHTING_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/core/container/darray.h"
#include "engine/resource/types.h"

typedef struct {
    arena_alloc_t *arena;
    vec4 ambient_color;

    darray_t lights;

    // The primary "sun" light
    light_t *sun;

    // Shadow map texture handle
    u32 shadow_map_fbo;
    u32 shadow_map_texture;
    u32 shadow_map_resolution;
} lighting_system_t;

lighting_system_t *light_sys_init(arena_alloc_t *arena);

void light_sys_kill(lighting_system_t *ls);

#endif // LIGHTING_H
