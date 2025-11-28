/**
 * @file arena.h
 * @brief Simple arena allocator for fast memory management
 *
 * This is my attempt at a memory arena - it's probably not the most efficient
 * but it's simple and gets the job done.
 */

#ifndef ARENA_ALLOC_H
#define ARENA_ALLOC_H

#include "define.h"

/**
 * @struct arena_alloc_t
 * @brief Arena allocator context
 *
 * A simple bump allocator that just moves a pointer forward.
 * No fancy free operations - just reset the whole thing when you're done!
 * Perfect for temporary allocations that die at the end of the frame.
 */
typedef struct {
    u64 total_size;
    u64 prev_offset;
    u64 curr_offset;

    void *memory;
    b8 own_memory;
} arena_alloc_t;

/**
 * @brief Create a new arena allocator
 * @param total_size Total size of the arena in bytes
 * @param arena Pointer to arena structure to initialize
 * @param memory Optional pre-allocated memory block (NULL to allocate new)
 * @return True if creation succeeded, false if something went wrong
 *
 * Sets up a new arena. If you pass your own memory, the arena won't free it.
 * If you don't, it'll allocate and manage the memory for you.
 */
b8 arena_create(u64 total_size, arena_alloc_t *arena, void *memory);

/**
 * @brief Destroy an arena and free its memory
 * @param arena Pointer to arena to destroy
 *
 * Cleans up the arena. If the arena owns its memory, it gets freed here.
 * If not, we just forget about it (someone else's problem!).
 */
void arena_kill(arena_alloc_t *arena);

/**
 * @brief Allocate aligned memory from the arena (default was 16 alignment)
 * @param arena Pointer to the arena
 * @param size Number of bytes to allocate
 * @param alignment Alignment requirement (power of two)
 * @return Pointer to allocated memory, or NULL if out of space
 *
 * The fancy version that handles alignment. Useful for SIMD or when you
 * need specific memory alignment for some reason.
 */
void *arena_alloc_align(arena_alloc_t *arena, u64 size, u8 alignment);

/**
 * @brief Allocate memory from the arena
 * @param arena Pointer to the arena
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL if out of space
 *
 * The simple version - just give me some bytes! Probably uses default
 * alignment. If this returns NULL, you're asking for too much memory (or
 * forgot to reset).
 */
void *arena_alloc(arena_alloc_t *arena, u64 size);

/**
 * @brief Reset the arena to its initial state
 * @param arena Pointer to arena to reset
 *
 * The "I'm done with all that memory" button. Doesn't actually free anything,
 * just moves the pointer back to the start. Super fast!
 */
void arena_reset(arena_alloc_t *arena);

/**
 * @brief Get remaining space in the arena
 * @param arena Pointer to the arena
 * @return Number of bytes still available
 *
 * How much memory you have left before you hit the wall and get NULL.
 */
u64 arena_remaining(const arena_alloc_t *arena);

/**
 * @brief Get used space in the arena
 * @param arena Pointer to the arena
 * @return Number of bytes currently allocated
 *
 * How much of your precious memory you've already used up.
 */
u64 arena_used(const arena_alloc_t *arena);

#endif // ARENA_ALLOC_H
