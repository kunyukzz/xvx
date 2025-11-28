#include "memory.h"

// std
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALLOC_TRACK 1024
#define BUFFER_SIZE 8192

struct status {
    u64 total_allocated;
    u64 tag_alloc_count[MEM_MAX_TAG];
    u64 tag_allocation[MEM_MAX_TAG];
};

typedef struct {
    memtag_t tag;
    u64 size;
    u32 line;
    const char *file;
    void *ptr;
} mem_state;

static struct status g_counter = {0};
static mem_state *g_mem;
static u64 g_mem_count = 0;
static u64 g_mem_capacity = 0;
static u64 g_mem_reserved = 0;

static const char *tag_str[MEM_MAX_TAG] = {
    "MEM_UNKNOWN", "MEM_GAME",  "MEM_ARENA",    "MEM_RENDER",
    "MEM_AUDIO",   "MEM_ARRAY", "MEM_DYNARRAY", "MEM_RESOURCE",
    "MEM_TEXTURE", "MEM_MESH",  "MEM_MATERIAL", "MEM_SHADER"};

static void memory_report_leaks(void)
{
    if (g_mem_count == 0)
    {
        LOG_INFO("No memory leaks detected.");
        return;
    }

    printf("\n");
    LOG_WARN("====== MEMORY LEAKS (%lu) ======", g_mem_count);
    for (u64 i = 0; i < g_mem_count; ++i)
    {
        const mem_state *m = &g_mem[i];
        LOG_WARN("at %s:%u → %lu bytes [%s]", m->file, m->line, m->size,
                 tag_str[m->tag]);
    }
}

b8 memory_sys_init(u64 total_size)
{
    // TODO: change malloc by platform allocation (mmap / virtual memory)
    g_mem_capacity = total_size / sizeof(mem_state);
    g_mem = malloc(sizeof(mem_state) * g_mem_capacity);
    if (!g_mem) return false;

    g_mem_count = 0;
    g_counter = (struct status){0};
    g_mem_reserved = total_size;

    LOG_INFO("Memory System Init");
    return true;
}

void memory_sys_kill(void)
{
    if (!g_mem) return;

    memory_report_leaks();
    free(g_mem);
    g_mem = 0;
    g_mem_count = 0;
    LOG_INFO("Memory System Kill");
}

void *alloc_dbg(u64 size, memtag_t tag, const char *file, u32 line)
{
    void *block = malloc(size);
    if (!block) return 0;

    memset(block, 0, size);
    if (g_mem_count < g_mem_capacity)
    {
        g_mem[g_mem_count++] = (mem_state){
            .ptr = block,
            .size = size,
            .tag = tag,
            .file = file,
            .line = line,
        };
    }

    g_counter.total_allocated += size;
    g_counter.tag_alloc_count[tag]++;
    g_counter.tag_allocation[tag] += size;

    return block;
}

void alloc_free(void *block, u64 size, memtag_t tag)
{
    if (!block) return;

    b8 found = false;
    for (u64 i = 0; i < g_mem_count; ++i)
    {
        if (g_mem[i].ptr == block)
        {
            g_mem[i] = g_mem[--g_mem_count];
            found = true;
            break;
        }
    }

    if (!found) LOG_WARN("attempted to free unknown ptr %p", block);
    free(block);

    g_counter.tag_alloc_count[tag]--;
    g_counter.tag_allocation[tag] -= size;
}

char *mem_debug_stat(void)
{
    const u64 Gib = 1024 * 1024 * 1024;
    const u64 Mib = 1024 * 1024;
    const u64 Kib = 1024;

    static char buffer[BUFFER_SIZE];
    u64 offset = 0;

    // Used vs Reserved
    f32 used_mib = (f32)g_counter.total_allocated / (f32)Mib;
    f32 reserved_mib = (f32)g_mem_reserved / (f32)Mib;

    offset += (u64)snprintf(buffer + offset, sizeof(buffer) - offset,
                            "Game Memory Used: %.2f Mib / %.2f Mib\n",
                            used_mib, reserved_mib);

    for (u32 i = 0; i < MEM_MAX_TAG; ++i)
    {
        char *unit = "B";
        u32 count = (u32)g_counter.tag_alloc_count[i];
        f32 amount = (f32)g_counter.tag_allocation[i];

        if (count == 0) continue;

        if (amount >= (f32)Gib)
        {
            amount /= (f32)Gib;
            unit = "Gib";
        }
        else if (amount >= (f32)Mib)
        {
            amount /= (f32)Mib;
            unit = "Mib";
        }
        else if (amount >= (f32)Kib)
        {
            amount /= (f32)Kib;
            unit = "Kib";
        }
        count = (u32)g_counter.tag_alloc_count[i];

        i32 length =
            snprintf(buffer + offset, sizeof(buffer) - offset,
                     "--> %s: [%u] %.2f%s\n", tag_str[i], count, amount, unit);

        if (length > 0 && (offset + (u32)length < BUFFER_SIZE))
        {
            offset += (u32)length;
        }
        else
            break;
    }
    return buffer;
}
