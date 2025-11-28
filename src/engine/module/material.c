#include "material.h"
#include "handle_util.h"
#include "engine/core/memory.h"

#include <string.h>

static material_system_t *g_mat = NULL;

material_system_t *material_sys_init(arena_alloc_t *arena, u32 capacity)
{
    material_system_t *mat = arena_alloc(arena, sizeof(material_system_t));
    if (!mat) return NULL;
    memset(mat, 0, sizeof(material_system_t));

    mat->arena = arena;
    mat->capacity = capacity;
    mat->count = 0;

    u64 total_size = sizeof(material_t) * capacity + sizeof(u16) * capacity +
                     sizeof(b8) * capacity;

    void *base_ptr = ALLOC(total_size, MEM_MATERIAL);
    if (!base_ptr) return NULL;

    mat->materials = (material_t *)base_ptr;
    mat->gen = (u16 *)(mat->materials + capacity);
    mat->used = (b8 *)(mat->gen + capacity);

    memset(mat->materials, 0, sizeof(material_t) * capacity);
    memset(mat->gen, 0, sizeof(u16) * capacity);
    memset(mat->used, 0, sizeof(b8) * capacity);

    mat->fl = freelist_create(arena, capacity);
    if (!mat->fl) return NULL;

    g_mat = mat;
    LOG_INFO("Material System Init");
    return mat;
}

void material_sys_kill(material_system_t *mat)
{
    if (!mat) return;

    freelist_destroy(mat->fl);

    u64 total_size = sizeof(material_t) * mat->capacity +
                     sizeof(u16) * mat->capacity + sizeof(b8) * mat->capacity;
    FREE(mat->materials, total_size, MEM_MATERIAL);

    memset(mat, 0, sizeof(material_system_t));
    LOG_INFO("Material System Kill");
}

material_handle_t material_create(const char *name, vec4 base_color,
                                  texture_handle_t albedo, f32 metallic,
                                  f32 roughness)
{
    if (!g_mat) return 0;

    u32 index = freelist_alloc(g_mat->fl);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free material slot available");
        return 0;
    }

    material_t *mat = &g_mat->materials[index];

    strncpy(mat->name, name, 63);
    mat->name[63] = '\0';
    mat->base_color = base_color;
    mat->albedo = albedo;
    mat->metallic = metallic;
    mat->roughness = roughness;

    g_mat->used[index] = true;
    g_mat->count++;

    u16 gen = g_mat->gen[index];
    material_handle_t handle = handle_create((u16)index, gen);

    LOG_DEBUG("created material: %s (handle: %u)", name, handle);

    return handle;
}

material_t *material_get(material_handle_t handle)
{
    if (!g_mat) return NULL;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    if (index >= g_mat->capacity)
    {
        LOG_ERROR("Index %u out of bounds", index);
        return NULL;
    }

    if (index >= g_mat->capacity) return NULL;

    if (!g_mat->used[index]) return NULL;

    if (g_mat->gen[index] != generation) return NULL;

    material_t *mat = &g_mat->materials[index];
    if (!mat) LOG_ERROR("failed to get material at index %u", index);

    return mat;
}

void material_destroy(material_handle_t handle)
{
    if (!g_mat) return;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    if (index >= g_mat->capacity)
    {
        LOG_WARN("Attempted to destroy invalid material handle: %u", handle);
        return;
    }

    if (g_mat->gen[index] != generation)
    {
        LOG_WARN("Attempted to destroy invalid material handle: %u", handle);
        return;
    }

    // Clear material data
    material_t *mat = &g_mat->materials[index];
    memset(mat, 0, sizeof(material_t));

    // Mark as unused and increment generation
    g_mat->gen[index]++;
    g_mat->used[index] = false;

    g_mat->count--;

    freelist_free(g_mat->fl, index);

    LOG_DEBUG("Destroyed material handle: %u", handle);
}
