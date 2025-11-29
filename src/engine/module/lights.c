#include "lights.h"
#include "handle_util.h"
#include "engine/core/memory.h"

#include <string.h>

static lighting_system_t *g_ls = NULL;

static const char *light_type_to_string(light_type_t type)
{
    switch (type)
    {
    case LIGHT_DIRECTIONAL: return "DIRECTIONAL";
    case LIGHT_POINT: return "POINT";
    case LIGHT_SPOT: return "SPOT";
    default: return "UNKNOWN";
    }
}

lighting_system_t *light_sys_init(arena_alloc_t *arena, u32 capacity)
{
    lighting_system_t *ls = arena_alloc(arena, sizeof(lighting_system_t));
    if (!ls) return NULL;
    memset(ls, 0, sizeof(lighting_system_t));

    ls->arena = arena;
    ls->capacity = capacity;
    ls->count = 0;

    u64 total_size = sizeof(light_t) * capacity + sizeof(u16) * capacity +
                     sizeof(b8) * capacity;

    void *base_ptr = ALLOC(total_size, MEM_LIGHT);
    if (!base_ptr) return NULL;

    ls->lights = (light_t *)(base_ptr);
    ls->gen = (u16 *)(ls->lights + capacity);
    ls->used = (b8 *)(ls->gen + capacity);

    memset(ls->lights, 0, sizeof(light_t) * capacity);
    memset(ls->gen, 0, sizeof(u16) * capacity);
    memset(ls->used, 0, sizeof(b8) * capacity);

    ls->fl = freelist_create(arena, capacity);
    if (!ls->fl) return NULL;

    g_ls = ls;

    ls->default_light =
        light_create(LIGHT_DIRECTIONAL, (vec3){{-0.7f, -0.5f, -0.5f}},
                     (vec3){{1.0f, 0.8f, 0.8f}}, 1.0f);

    LOG_INFO("Light System Init");
    return ls;
}

void light_sys_kill(lighting_system_t *ls)
{
    if (!ls) return;

    light_destroy(g_ls->default_light);

    freelist_destroy(ls->fl);

    u64 total_size = sizeof(light_t) * ls->capacity +
                     sizeof(u16) * ls->capacity + sizeof(b8) * ls->capacity;
    FREE(ls->lights, total_size, MEM_LIGHT);

    memset(ls, 0, sizeof(lighting_system_t));
    LOG_INFO("Light System Kill");
}

light_handle_t light_create(light_type_t type, vec3 pos, vec3 color,
                            f32 intensity)
{
    if (!g_ls) return 0;

    u32 index = freelist_alloc(g_ls->fl);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free light slot available");
        return 0;
    }
    LOG_DEBUG("Allocated light at index %u (free: %u)", index,
              freelist_available_count(g_ls->fl));

    light_t *l = &g_ls->lights[index];
    memset(l, 0, sizeof(light_t));

    // Initialize with sensible defaults
    l->type = type;
    l->position = pos;
    l->color = color;
    l->intensity = intensity;
    l->enable = true;

    switch (type)
    {
    case LIGHT_DIRECTIONAL: l->radius = 0.0f; break;
    case LIGHT_POINT: l->radius = 10.0f; break;
    case LIGHT_SPOT:
        l->radius = 15.0f;
        l->inner_cutoff = 0.9f; // Cosine of angle
        l->outer_cutoff = 0.8f;
        break;
    }

    g_ls->used[index] = true;
    g_ls->count++;

    u16 gen = g_ls->gen[index];
    light_handle_t handle = handle_create((u16)index, gen);

    LOG_DEBUG("Created %s light at (%.2f, %.2f, %.2f)",
              light_type_to_string(type), pos.x, pos.y, pos.z);

    return handle;
}

void light_destroy(light_handle_t handle)
{
    light_t *l = light_get(handle);
    if (!l) return;

    u16 index = handle_get_index(handle);

    memset(l, 0, sizeof(light_t));

    freelist_free(g_ls->fl, index);

    g_ls->used[index] = false;
    g_ls->count--;
    g_ls->gen[index]++;
}

light_t *light_get(light_handle_t handle)
{
    if (!g_ls) return NULL;

    if (handle == 0) handle = g_ls->default_light;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    if (index >= g_ls->capacity)
    {
        LOG_ERROR("Index %u out of bounds", index);
        return NULL;
    }

    if (!g_ls->used[index]) return NULL;

    if (g_ls->gen[index] != generation) return NULL;

    light_t *light = &g_ls->lights[index];
    if (!light)
    {
        LOG_ERROR("cannot get light");
    }

    return light;
}
