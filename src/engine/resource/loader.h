/**
 * @file loader.h
 * @brief Resource loading utilities for files and images
 *
 * Helper functions for loading binary files, text files, and images.
 * Uses stb_image for image loading because writing image decoders is hard.
 * All functions use the engine's memory system with proper tagging.
 */

#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include "engine/core/define.h"
#include "engine/core/memory.h"
#include "engine/platform/filesystem.h"
#include "deps/stb_image/stb_image.h"

/**
 * @brief Read entire file as binary data
 * @param path Path to the file
 * @param out_size Output for file size in bytes
 * @return Pointer to allocated buffer with file data, or NULL if failed
 *
 * Reads a file completely into memory using the engine's allocator.
 * The returned buffer must be freed with FREE() when you're done with it!
 * Uses MEM_RESOURCE tag for memory tracking.
 */
INL void *read_file_binary(const char *path, u64 *out_size)
{
    file_t file;
    if (!file_open(path, READ_BINARY, &file))
    {
        LOG_ERROR("Failed to open binary file: %s", path);
        return NULL;
    }

    u64 size = 0;
    if (!file_size(&file, &size) || size == 0)
    {
        LOG_ERROR("Binary file is empty: %s", path);
        file_close(&file);
        return NULL;
    }

    u8 *data = ALLOC(size, MEM_RESOURCE);
    u64 read_size = 0;
    if (!file_read_all_binary(&file, data, &read_size) || read_size != size)
    {
        LOG_ERROR("Failed to read binary file: %s", path);
        file_close(&file);
        FREE(data, size, MEM_RESOURCE);
        return NULL;
    }

    file_close(&file);
    *out_size = size;
    return data;
}

/**
 * @brief Read entire file as text
 * @param path Path to the file
 * @param out_size Output for file size in bytes (excluding null terminator)
 * @return Pointer to allocated null-terminated string, or NULL if failed
 *
 * Reads a text file and null-terminates it. Useful for shaders, config files,
 * etc. The returned string must be freed with FREE() when you're done! Uses
 * MEM_RESOURCE tag for memory tracking.
 */
INL void *read_file_text(const char *path, u64 *out_size)
{
    file_t file;
    if (!file_open(path, READ_TEXT, &file))
    {
        LOG_ERROR("Failed to open text file: %s", path);
        return NULL;
    }

    u64 size = 0;
    if (!file_size(&file, &size) || size == 0)
    {
        LOG_ERROR("Text file is empty: %s", path);
        file_close(&file);
        return NULL;
    }

    char *data = ALLOC(size, MEM_RESOURCE);
    u64 read_size = 0;
    if (!file_read_all_text(&file, data, &read_size) || read_size != size)
    {
        LOG_ERROR("Failed to read text file: %s", path);
        file_close(&file);
        FREE(data, size, MEM_RESOURCE);
        return NULL;
    }

    data[size] = '\0';
    file_close(&file);
    *out_size = size;
    return data;
}

/**
 * @brief Load image file and decode pixels
 * @param path Path to the image file
 * @param width Output for image width
 * @param height Output for image height
 * @param channels Output for number of color channels
 * @return Pointer to decoded pixel data, or NULL if failed
 *
 * Uses stb_image to load common image formats (PNG, JPG, etc.).
 * Always returns RGBA format (4 channels) for consistency.
 * Flips image vertically because OpenGL expects bottom-left origin.
 * The returned pixel data must be freed with stbi_image_free()!
 */
INL void *read_image_file(const char *path, i32 *width, i32 *height,
                          i32 *channels)
{
    file_t file;
    if (!file_open(path, READ_BINARY, &file))
    {
        LOG_ERROR("Failed to open image file: %s", path);
        return NULL;
    }

    u64 size = 0;
    if (!file_size(&file, &size))
    {
        LOG_ERROR("Image file is empty: %s", path);
        file_close(&file);
        return NULL;
    }

    u8 *data = ALLOC(size, MEM_RESOURCE);
    u64 read_size = 0;
    if (!file_read_all_binary(&file, data, &read_size) || read_size != size)
    {
        LOG_ERROR("Failed to read image file: %s", path);
        file_close(&file);
        FREE(data, size, MEM_RESOURCE);
        return NULL;
    }
    file_close(&file);

    stbi_set_flip_vertically_on_load(true);
    u8 *pixels = stbi_load_from_memory(data, (i32)size, width, height,
                                       channels, STBI_rgb_alpha);
    if (pixels) *channels = 4;

    FREE(data, size, MEM_RESOURCE);

    if (!pixels) return NULL;

    return pixels;
}

#endif // RESOURCE_LOADER_H
