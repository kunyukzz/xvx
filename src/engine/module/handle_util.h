/**
 * @file handle_util.h
 * @brief Handle utilities for resource management
 *
 * Simple handle system that packs index and generation into 32 bits.
 * This helps prevent use-after-free by checking generations.
 * Or at least that's the theory - we'll see how it works in practice!
 */

#ifndef HANDLE_UTIL_H
#define HANDLE_UTIL_H

#include "engine/core/define.h"

/**
 * @brief Extract the index from a handle
 * @param handle The packed handle value
 * @return The index part (lower 16 bits)
 *
 * Gets which slot in the array this handle refers to.
 */
INL u16 handle_get_index(u32 handle) { return handle & INVALID_16; }

/**
 * @brief Extract the generation from a handle
 * @param handle The packed handle value
 * @return The generation part (upper 16 bits)
 *
 * Gets the generation counter to detect stale handles.
 */
INL u16 handle_get_generation(u32 handle)
{
    return (handle >> 16) & INVALID_16;
}

/**
 * @brief Create a new handle from index and generation
 * @param index Array index (0-65535)
 * @param gen Generation counter
 * @return Packed 32-bit handle
 *
 * Packs index and generation into one 32-bit value.
 */
INL u32 handle_create(u16 index, u16 gen)
{
    return ((u32)gen << 16) | (u32)index;
}

#endif // HANDLE_UTIL_H
