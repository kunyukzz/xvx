#include "lights.h"

#include <string.h>

lighting_system_t *light_sys_init(arena_alloc_t *arena)
{
    lighting_system_t *ls = arena_alloc(arena, sizeof(lighting_system_t));
    if (!ls) return NULL;
    memset(ls, 0, sizeof(lighting_system_t));

    ls->arena = arena;

    LOG_INFO("Light System Init");
    return ls;
}

void light_sys_kill(lighting_system_t *ls)
{
    if (!ls) return;

    memset(ls, 0, sizeof(lighting_system_t));
    LOG_INFO("Light System Kill");
}
