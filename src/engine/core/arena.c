#include "arena.h"
#include "memory.h"

#include <string.h>

#define DEFAULT_ALIGNMENT 0x10

b8 arena_create(u64 total_size, arena_alloc_t *arena, void *memory)
{
    if (!arena) return false;

    arena->total_size = total_size;
    arena->prev_offset = 0;
    arena->curr_offset = 0;
    arena->own_memory = memory == NULL;

    if (!memory)
    {
        arena->memory = ALLOC(total_size, MEM_ARENA);
        if (!arena->memory) return false;
    }
    else
    {
        arena->memory = memory;
    }

    return true;
}

void arena_kill(arena_alloc_t *arena)
{
    if (!arena) return;

    if (arena->memory) FREE(arena->memory, arena->total_size, MEM_ARENA);
    memset(arena, 0, sizeof(*arena));
}

void *arena_alloc_align(arena_alloc_t *arena, u64 size, u8 alignment)
{
    if (!arena || size == 0) return NULL;

    u64 aligned_offset =
        (arena->curr_offset + (alignment - 1)) & ~((u64)alignment - 1);

    if (aligned_offset + size > arena->total_size) return NULL;

    arena->prev_offset = arena->curr_offset;
    arena->curr_offset = aligned_offset + size;

    return (u8 *)arena->memory + aligned_offset;
}

void *arena_alloc(arena_alloc_t *arena, u64 size)
{
    return arena_alloc_align(arena, size, DEFAULT_ALIGNMENT);
}

void arena_reset(arena_alloc_t *arena)
{
    if (arena)
    {
        arena->prev_offset = 0;
        arena->curr_offset = 0;
    }
}

u64 arena_remaining(const arena_alloc_t *arena)
{
    return arena ? (arena->total_size - arena->curr_offset) : 0;
}

u64 arena_used(const arena_alloc_t *arena)
{
    return arena ? arena->curr_offset : 0;
}
