#include "mesh.h"
#include "engine/core/memory.h"
#include "engine/core/container/freelist.h"
#include "engine/rendering/renderer.h"
#include "engine/resource/geometry.h"
#include "handle_util.h"

// std
#include <string.h>

static mesh_system_t *g_mesh = NULL;

mesh_system_t *mesh_sys_init(arena_alloc_t *arena, u32 capacity)
{
    mesh_system_t *mesh = arena_alloc(arena, sizeof(mesh_system_t));
    if (!mesh) return NULL;
    memset(mesh, 0, sizeof(mesh_system_t));

    mesh->arena = arena;
    mesh->capacity = capacity;
    mesh->count = 0;
    mesh->internal_id = 1;

    u64 total_size = sizeof(mesh_t) * capacity + sizeof(u16) * capacity +
                     sizeof(b8) * capacity;

    void *base_ptr = ALLOC(total_size, MEM_MESH);
    if (!base_ptr) return NULL;

    mesh->meshes = (mesh_t *)(base_ptr);
    mesh->gen = (u16 *)(mesh->meshes + capacity);
    mesh->used = (b8 *)(mesh->gen + capacity);

    memset(mesh->meshes, 0, sizeof(mesh_t) * capacity);
    memset(mesh->gen, 0, sizeof(u16) * capacity);
    memset(mesh->used, 0, sizeof(b8) * capacity);

    mesh->fl = freelist_create(arena, capacity);
    if (!mesh->fl) return NULL;

    g_mesh = mesh;
    LOG_INFO("Mesh System Init -> %u slots", capacity);
    return mesh;
}

void mesh_sys_kill(mesh_system_t *mesh)
{
    if (!mesh) return;

    freelist_destroy(mesh->fl);

    u64 total_size = sizeof(mesh_t) * mesh->capacity +
                     sizeof(u16) * mesh->capacity +
                     sizeof(b8) * mesh->capacity;
    FREE(mesh->meshes, total_size, MEM_MESH);

    memset(mesh, 0, sizeof(mesh_system_t));
    LOG_INFO("Mesh System Kill");
}

mesh_handle_t mesh_create_plane(float width, float height, u32 segments)
{
    if (!g_mesh) return 0;
    u32 index = freelist_alloc(g_mesh->fl);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free mesh slot available");
        return 0;
    }
    LOG_DEBUG("Allocated mesh at index %u (free: %u)", index,
              freelist_available_count(g_mesh->fl));

    mesh_t *m = &g_mesh->meshes[index];
    m->uploaded = false;
    m->internal = NULL;

    geometry_t geo = geometry_gen_plane(width, height, segments);
    m->geometry = geo;
    m->uploaded = true;
    m->id = g_mesh->internal_id++;

    g_mesh->used[index] = true;
    g_mesh->count++;

    u16 gen = g_mesh->gen[index];
    mesh_handle_t handle = handle_create((u16)index, gen);

    render_upload_mesh(handle, &geo);
    geometry_destroy(&geo);
    LOG_DEBUG("created mesh: %u (handle: %u - sort id: %u)", index, handle,
              g_mesh->internal_id);

    return handle;
}

mesh_handle_t mesh_create_cube(f32 width, f32 height, f32 depth)
{
    if (!g_mesh) return 0;
    u32 index = freelist_alloc(g_mesh->fl);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free mesh slot available");
        return 0;
    }
    /*
    LOG_DEBUG("Allocated mesh at index %u (free: %u)", index,
              freelist_available_count(g_mesh->fl));
              */

    mesh_t *m = &g_mesh->meshes[index];
    m->uploaded = false;
    m->uploaded = NULL;

    geometry_t geo = geometry_gen_cube(width, height, depth);
    m->geometry = geo;
    m->uploaded = true;
    m->id = g_mesh->internal_id++;

    g_mesh->used[index] = true;
    g_mesh->count++;

    u16 gen = g_mesh->gen[index];
    mesh_handle_t handle = handle_create((u16)index, gen);

    render_upload_mesh(handle, &geo);
    geometry_destroy(&geo);

    LOG_DEBUG("created mesh: %u (handle: %u - sort id: %u)", index, handle,
              g_mesh->internal_id);

    return handle;
}

void mesh_destroy(mesh_handle_t handle)
{
    if (!g_mesh) return;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    if (index >= g_mesh->capacity)
    {
        LOG_WARN("Attempted to destroy invalid mesh handle: %u", handle);
        return;
    }

    if (g_mesh->gen[index] != generation)
    {
        LOG_WARN("Attempted to destroy invalid mesh handle: %u", handle);
        return;
    }

    // Cleanup mesh resources...
    mesh_t *mesh = &g_mesh->meshes[index];
    geometry_destroy(&mesh->geometry);
    if (mesh->internal)
    {
        // TODO: implement this!!!!
        // renderer_destroy_mesh(handle);
    }

    // Clear the slot
    memset(mesh, 0, sizeof(mesh_t));
    g_mesh->gen[index]++;
    g_mesh->used[index] = false;

    if (!freelist_free(g_mesh->fl, index))
    {
        LOG_ERROR("Failed to free mesh index %u", index);
    }

    g_mesh->count--;
    LOG_DEBUG("Freed mesh index %u (free: %u)", index,
              freelist_available_count(g_mesh->fl));
}

mesh_t *mesh_get(mesh_handle_t handle)
{
    if (!g_mesh) return NULL;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    /*
    LOG_DEBUG("mesh_get - handle: %u, index: %u, generation: %u", handle,
              index, generation);
              */

    if (index >= g_mesh->capacity)
    {
        LOG_ERROR("Index %u out of bounds ", index);
        return NULL;
    }

    if (index >= g_mesh->capacity) return NULL;

    if (!g_mesh->used[index]) return NULL;

    if (g_mesh->gen[index] != generation) return NULL;

    mesh_t *mesh = &g_mesh->meshes[index];
    if (!mesh)
    {
        LOG_ERROR("failed to get mesh at index %u", index);
    }

    return mesh;
}
