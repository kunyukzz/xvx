/**
 * @file freelist.h
 * @brief Free list allocator for efficient slot management
 *
 * A LIFO (Last-In-First-Out) free list that tracks available indices.
 * Used by all the object pool systems (mesh, texture, material, etc.)
 * to efficiently allocate and free slots in fixed-size arrays.
 */

#ifndef FREELIST_H
#define FREELIST_H

#include "engine/core/define.h"

/**
 * @struct freelist_t
 * @brief Free list structure for managing available indices
 *
 * Maintains a stack of available indices that can be allocated.
 * Uses LIFO order because it's simple and fast.
 * The arena is used for storing the index array.
 */
typedef struct {
    void *arena;
    u32 *indices;
    u32 capacity;
    u32 top;
    u32 allocated;
} freelist_t;

/**
 * @brief Create a new free list
 * @param arena Memory arena for the index array
 * @param capacity Maximum number of indices to manage
 * @return Pointer to new free list, or NULL if failed
 *
 * Initializes a free list with indices 0 through capacity-1.
 * All indices start as available.
 */
freelist_t *freelist_create(void *arena, u32 capacity);

/**
 * @brief Destroy a free list
 * @param fl Pointer to free list to destroy
 *
 * Cleans up the free list. Does not free the arena memory.
 */
void freelist_destroy(freelist_t *fl);

/**
 * @brief Allocate an index from the free list
 * @param fl Pointer to free list
 * @return Allocated index, or (u32)-1 if free list is empty
 *
 * Pops an available index from the top of the stack (LIFO).
 * Returns (u32)-1 if no indices are available.
 */
u32 freelist_alloc(freelist_t *fl);

/**
 * @brief Free an index back to the free list
 * @param fl Pointer to free list
 * @param index Index to free
 * @return True if index was successfully freed, false if invalid
 *
 * Pushes an index back onto the free list stack.
 * The index must be within the capacity range.
 */
b8 freelist_free(freelist_t *fl, u32 index);

/**
 * @brief Get the total capacity of the free list
 * @param fl Pointer to free list
 * @return Maximum number of indices this free list can manage
 */
u32 freelist_capacity(const freelist_t *fl);

/**
 * @brief Get the number of allocated indices
 * @param fl Pointer to free list
 * @return Number of currently allocated indices
 */
u32 freelist_allocated_count(const freelist_t *fl);

/**
 * @brief Get the number of available indices
 * @param fl Pointer to free list
 * @return Number of indices currently available for allocation
 */
u32 freelist_available_count(const freelist_t *fl);

/**
 * @brief Check if the free list is full
 * @param fl Pointer to free list
 * @return True if all indices are allocated (none available)
 */
b8 freelist_is_full(const freelist_t *fl);

/**
 * @brief Check if the free list is empty
 * @param fl Pointer to free list
 * @return True if no indices are allocated (all available)
 */
b8 freelist_is_empty(const freelist_t *fl);

#endif // FREELIST_H
