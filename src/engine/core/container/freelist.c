#include "freelist.h"
#include "engine/core/arena.h"

// std
#include <string.h>

freelist_t *freelist_create(void *arena, u32 capacity)
{
    // Validate parameters
    if (!arena || capacity == 0) return NULL;

    // Allocate the free list structure from the arena
    freelist_t *fl = arena_alloc(arena, sizeof(freelist_t));
    if (!fl) return NULL;

    fl->arena = arena;
    fl->capacity = capacity;
    fl->top = 0;
    fl->allocated = 0;

    // Allocate the index stack from the arena
    // This array will hold all the free indices in stack order
    fl->indices = arena_alloc(arena, capacity * sizeof(u32));
    if (!fl->indices) return NULL;

    // Initialize the free list with all indices [0, capacity-1]
    // We fill the stack in reverse order so that index 0 is at the bottom
    // and the highest index is at the top (will be allocated first)
    for (u32 i = 0; i < capacity; ++i) fl->indices[fl->top++] = i;

    return fl;
}

void freelist_destroy(freelist_t *fl)
{
    // The entire arena will be reset or destroyed by the owner
    // This function is mainly for symmetry and future-proofing
    if (fl)
    {
        // Could zero the memory for safety, but arena reset handles
        memset(fl, 0, sizeof(freelist_t));
    }
}

u32 freelist_alloc(freelist_t *fl)
{
    if (!fl) return INVALID_32;

    // Check if we have any free indices left
    if (fl->top == 0) return INVALID_32;

    // Pop the top index from the stack (LIFO behavior)
    // Initialized with highest indices last, allocate
    // from highest to lowest: 1023, 1022, 1021, ... 0
    u32 index = fl->indices[--fl->top];
    fl->allocated++;

    return index;
}

b8 freelist_free(freelist_t *fl, u32 index)
{
    if (!fl) return false;

    // Validate the index is within bounds
    if (index >= fl->capacity) return false;

    // Check if we have room in the stack
    if (fl->top >= fl->capacity)
        return false; // Stack overflow - should never happen

    // Push the index back onto the free stack
    fl->indices[fl->top++] = index;
    fl->allocated--;

    return true;
}

u32 freelist_capacity(const freelist_t *fl) { return fl ? fl->capacity : 0; }

u32 freelist_allocated_count(const freelist_t *fl)
{
    return fl ? fl->allocated : 0;
}

u32 freelist_available_count(const freelist_t *fl)
{
    return fl ? fl->top : 0; // Top indicates how many are free
}

b8 freelist_is_full(const freelist_t *fl)
{
    return fl ? (fl->top == 0) : true;
}

b8 freelist_is_empty(const freelist_t *fl)
{
    return fl ? (fl->allocated == 0) : true;
}
