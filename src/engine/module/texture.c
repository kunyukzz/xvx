#include "texture.h"
#include "engine/core/container/freelist.h"
#include "engine/rendering/renderer.h"
#include "engine/resource/loader.h"
#include "handle_util.h"

// std
#include <string.h>

static texture_handle_t create_checker_texture(void)
{
    u32 size = 64;
    u8 pixels[size * size * 3]; // RGB

    for (u32 y = 0; y < size; y++)
    {
        for (u32 x = 0; x < size; x++)
        {
            b8 dark = ((x / 8) + (y / 8)) % 2 == 0;
            u32 idx = (y * size + x) * 3;

            if (dark)
            {
                pixels[idx] = 100;     // R - dark gray
                pixels[idx + 1] = 100; // G
                pixels[idx + 2] = 100; // B
            }
            else
            {
                pixels[idx] = 200;     // R - light gray
                pixels[idx + 1] = 200; // G
                pixels[idx + 2] = 200; // B
            }
        }
    }

    return texture_create_from_data(size, size, 3, pixels);
}

static texture_system_t *g_ts = NULL;

texture_system_t *texture_sys_init(arena_alloc_t *arena, u32 capacity)
{
    texture_system_t *ts = arena_alloc(arena, sizeof(texture_system_t));
    if (!ts) return NULL;
    memset(ts, 0, sizeof(texture_system_t));

    ts->arena = arena;
    ts->capacity = capacity;
    ts->count = 0;

    u64 total_size = sizeof(texture_t) * capacity + sizeof(u16) * capacity +
                     sizeof(b8) * capacity;

    void *base_ptr = ALLOC(total_size, MEM_TEXTURE);
    if (!base_ptr) return NULL;

    ts->textures = (texture_t *)(base_ptr);
    ts->gen = (u16 *)(ts->textures + capacity);
    ts->used = (b8 *)(ts->gen + capacity);

    memset(ts->textures, 0, sizeof(texture_t) * capacity);
    memset(ts->gen, 0, sizeof(u16) * capacity);
    memset(ts->used, 0, sizeof(b8) * capacity);

    ts->fl = freelist_create(arena, capacity);
    if (!ts->fl) return NULL;

    for (u32 i = 0; i < MAX_TEXTURE_UNIT; ++i)
    {
        ts->bound_textures[i] = INVALID_32;
    }
    ts->active_unit = 0;

    g_ts = ts;
    ts->default_texture = create_checker_texture();

    LOG_INFO("Texture System Init");
    return ts;
}

void texture_sys_kill(texture_system_t *ts)
{
    if (!ts) return;

    freelist_destroy(ts->fl);

    u64 total_size = sizeof(texture_t) * ts->capacity +
                     sizeof(u16) * ts->capacity + sizeof(b8) * ts->capacity;
    FREE(ts->textures, total_size, MEM_TEXTURE);

    for (u32 i = 0; i < MAX_TEXTURE_UNIT; ++i)
    {
        ts->bound_textures[i] = INVALID_32;
    }
    ts->active_unit = 0;

    memset(ts, 0, sizeof(texture_system_t));
    LOG_INFO("Texture System Kill");
}

texture_handle_t texture_create_from_file(const char *path)
{
    if (!g_ts) return 0;

    u32 index = freelist_alloc(g_ts->fl);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free texture slot available");
        return 0;
    }

    i32 width, height, channels;
    char name[MAX_PATH];
    snprintf(name, sizeof(name), "%s.png", path);
    void *pixel = read_image_file(name, &width, &height, &channels);
    if (!pixel)
    {
        freelist_free(g_ts->fl, index);
        return 0;
    }

    texture_t *t = &g_ts->textures[index];
    strcpy(t->name, path);
    t->width = (u32)width;
    t->height = (u32)height;
    t->channels = (u32)channels;

    LOG_DEBUG("Texture: %s, %ux%u, channels: %u", t->name, t->width, t->height,
              t->channels);

    g_ts->used[index] = true;
    g_ts->count++;

    u16 gen = g_ts->gen[index];
    texture_handle_t handle = handle_create((u16)index, gen);
    LOG_DEBUG("Handle creation - index: %u, generation: %u, handle: %u", index,
              gen, handle);

    // Upload to GPU
    render_upload_texture(handle, pixel);
    stbi_image_free(pixel);

    return handle;
}

texture_handle_t texture_create_from_data(u32 width, u32 height, u32 channels,
                                          void *pixel)
{
    if (!g_ts || !pixel) return 0;

    u32 index = freelist_alloc(g_ts->fl);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free texture slot available");
        return 0;
    }

    texture_t *t = &g_ts->textures[index];

    snprintf(t->name, sizeof(t->name), "generated_%ux%u", width, height);
    t->width = width;
    t->height = height;
    t->channels = channels;

    g_ts->used[index] = true;
    g_ts->count++;

    u16 gen = g_ts->gen[index];
    texture_handle_t handle = handle_create((u16)index, gen);
    LOG_DEBUG("Created texture from data: %u (%ux%u, channels: %u)", handle,
              width, height, channels);

    // Upload to GPU
    render_upload_texture(handle, pixel);
    return handle;
}

texture_t *texture_get(texture_handle_t handle)
{
    if (!g_ts) return NULL;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    if (index >= g_ts->capacity) return NULL;

    if (!g_ts->used[index]) return NULL;

    if (g_ts->gen[index] != generation) return NULL;

    return &g_ts->textures[index];
}

void texture_bind(texture_handle_t handle, u32 unit)
{
    if (!g_ts) return;

    if (unit >= MAX_TEXTURE_UNIT)
    {
        LOG_ERROR("Texture unit %u out of bounds", unit);
        return;
    }

    if (g_ts->bound_textures[unit] == handle) return;

    texture_t *tex = texture_get(handle);
    if (!tex)
    {
        tex = texture_get(g_ts->default_texture);
        if (!tex) return;
        handle = g_ts->default_texture;
    }

    g_ts->bound_textures[unit] = handle;

    if (g_ts->active_unit != unit)
    {
        g_ts->active_unit = unit;
        render_set_active_texture(unit);
    }

    render_bind_texture(handle, unit);
}

void texture_bind_active(texture_handle_t handle)
{
    texture_bind(handle, g_ts->active_unit);
}

texture_handle_t texture_get_default(void)
{
    return g_ts ? g_ts->default_texture : 0;
}
