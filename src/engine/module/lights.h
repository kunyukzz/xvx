#ifndef LIGHTING_H
#define LIGHTING_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/core/container/freelist.h"
#include "engine/resource/types.h"

typedef struct {
    arena_alloc_t *arena;
    u32 capacity;
    u32 count;

    light_t *lights;
    u16 *gen;
    b8 *used;

    freelist_t *fl;

    light_handle_t default_light;
} lighting_system_t;

lighting_system_t *light_sys_init(arena_alloc_t *arena, u32 capacity);

void light_sys_kill(lighting_system_t *ls);

light_handle_t light_create(light_type_t type, vec3 pos, vec3 color,
                            f32 intensity);

void light_destroy(light_handle_t handle);

light_t *light_get(light_handle_t handle);

#endif // LIGHTING_H
