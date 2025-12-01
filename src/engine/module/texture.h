/**
 * @file texture.h
 * @brief Texture system for image loading and management
 *
 * Handles texture creation from files or memory, and GPU upload.
 * Uses stb_image under the hood because writing image loaders is hard.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/core/container/freelist.h"
#include "engine/resource/types.h"

#define MAX_TEXTURE_UNIT 8

/**
 * @struct texture_system_t
 * @brief Internal texture system state
 *
 * Same object pool pattern because copy-paste is efficient!
 * Has a default texture (a beautiful 64x64 checkerboard) for fallback.
 */
typedef struct {
    arena_alloc_t *arena;
    u32 capacity;
    u32 count;

    texture_t *textures;
    u16 *gen;
    b8 *used;

    freelist_t *fl;

    texture_handle_t default_texture;

    texture_handle_t bound_textures[MAX_TEXTURE_UNIT];
    u32 active_unit;
} texture_system_t;

/**
 * @brief Initialize the texture system
 * @param arena Memory arena for allocations
 * @param capacity Maximum number of textures
 * @return Pointer to texture system, or NULL if failed
 *
 * Sets up the texture system and creates the default texture.
 * The default texture is a gorgeous 64x64 checkerboard because
 * missing textures should at least look interesting!
 */
texture_system_t *texture_sys_init(arena_alloc_t *arena, u32 capacity);

/**
 * @brief Shutdown the texture system
 * @param ts Pointer to texture system
 *
 * Deletes all textures and frees GPU memory.
 * OpenGL texture handles become invalid after this.
 */
void texture_sys_kill(texture_system_t *ts);

/**
 * @brief Create a texture from an image file
 * @param path Path to the image file
 * @return Handle to the created texture
 *
 * Loads an image using stb_image and uploads it to GPU.
 * Supports common formats (PNG & JPG) but not exotic ones.
 * If loading fails, you get the awesome checkerboard instead!
 */
texture_handle_t texture_create_from_file(const char *path);

/**
 * @brief Create a texture from raw pixel data
 * @param width Texture width in pixels
 * @param height Texture height in pixels
 * @param channels Number of color channels (1-4)
 * @param pixel Pointer to raw pixel data
 * @return Handle to the created texture
 *
 * Creates a texture from existing memory. Useful for procedural textures
 * or when you have image data from other sources.
 */
texture_handle_t texture_create_from_data(u32 width, u32 height, u32 channels,
                                          void *pixel);

/**
 * @brief Get texture data from a handle
 * @param handle Handle to the texture
 * @return Pointer to texture data, or NULL if invalid
 *
 * Useful for getting texture dimensions or other properties.
 * If you get the default texture, admire that checkerboard pattern!
 */
texture_t *texture_get(texture_handle_t handle);

/**
 * @brief Get the default texture handle
 * @return Handle to the default texture
 *
 * Returns the magnificent checkerboard texture (64x64 RGB).
 * Perfect for when you don't have a real texture but want some visual flair!
 */
texture_handle_t texture_get_default(void);

void texture_bind(texture_handle_t handle, u32 unit);

void texture_bind_active(texture_handle_t handle);

#endif // TEXTURE_H
