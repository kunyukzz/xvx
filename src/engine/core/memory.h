/**
 * @file memory.h
 * @brief Memory allocation with basic tracking and debugging
 *
 * My attempt at memory management with some basic tagging for debugging.
 * It's not a full-blown memory debugger, but it helps me see where memory go.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "define.h"

/**
 * @enum memtag_t
 * @brief Memory allocation tags for tracking where memory goes
 *
 * These tags help me figure out what's eating all the RAM when things go
 * wrong. The names are mostly guesses about what each system actually uses
 * memory for.
 */
typedef enum {
    MEM_UNKNOWN = 0x00,
    MEM_GAME,
    MEM_ARENA,
    MEM_RENDER,
    MEM_AUDIO,
    MEM_ARRAY,
    MEM_DYNARRAY,
    MEM_RESOURCE,
    MEM_TEXTURE,
    MEM_MESH,
    MEM_MATERIAL,
    MEM_SHADER,
    MEM_MAX_TAG
} memtag_t;

/**
 * @def ALLOC(size, tag)
 * @brief Allocate memory with debugging info
 * @param size Number of bytes to allocate
 * @param tag Memory tag for tracking
 *
 * The fancy allocator that remembers where you called it from.
 */
#define ALLOC(size, tag) alloc_dbg(size, tag, __FILE__, __LINE__)

/**
 * @def FREE(block, size, tag)
 * @brief Free previously allocated memory
 * @param block Pointer to memory block
 * @param size Size of the block (for verification)
 * @param tag Tag that was used during allocation
 *
 * Free memory and update the tracking stats.
 */
#define FREE(block, size, tag) alloc_free(block, size, tag)

/**
 * @brief Initialize the memory system
 * @param total_size Total memory to pre-allocate (or something like that)
 * @return True if initialization succeeded
 *
 * Sets up whatever backing store we're using. Or maybe just initializes stats.
 */
b8 memory_sys_init(u64 total_size);

/**
 * @brief Shutdown the memory system
 *
 * Cleans up and hopefully reports any memory leaks.
 * If this doesn't crash, we probably didn't leak too much memory.
 */
void memory_sys_kill(void);

/**
 * @brief Debug allocation function
 * @param size Bytes to allocate
 * @param tag Memory tag for tracking
 * @param file Source file where allocation occurred
 * @param line Line number where allocation occurred
 * @return Pointer to allocated memory, or NULL if failed
 *
 * The real workhorse - allocates memory and tracks where it came from.
 */
void *alloc_dbg(u64 size, memtag_t tag, const char *file, u32 line);

/**
 * @brief Free allocated memory
 * @param block Memory block to free
 * @param size Size of the block (for stats)
 * @param tag Tag used during allocation
 *
 * Frees memory and updates the tracking statistics.
 */
void alloc_free(void *block, u64 size, memtag_t tag);

/**
 * @brief Get memory usage statistics as a string
 * @return Formatted string with memory usage per tag
 *
 * This is my favorite function - it tells me how bad the memory situation is.
 */
char *mem_debug_stat(void);

#endif // MEMORY_H
