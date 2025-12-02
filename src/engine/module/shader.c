#include "shader.h"
#include "engine/core/memory.h"
#include "engine/rendering/renderer.h"
#include "handle_util.h"

// std
#include <string.h>

static shader_system_t *g_sh = NULL;

shader_system_t *shader_sys_init(arena_alloc_t *arena, u32 capacity)
{
    shader_system_t *sh = arena_alloc(arena, sizeof(shader_system_t));
    if (!sh) return NULL;
    memset(sh, 0, sizeof(shader_system_t));

    sh->arena = arena;
    sh->capacity = capacity;
    sh->count = 0;

    u64 total_size = sizeof(shader_t) * capacity + sizeof(u16) * capacity +
                     sizeof(b8) * capacity;

    void *base_ptr = ALLOC(total_size, MEM_SHADER);
    if (!base_ptr) return NULL;

    sh->shaders = (shader_t *)base_ptr;
    sh->gen = (u16 *)(sh->shaders + capacity);
    sh->used = (b8 *)(sh->gen + capacity);

    memset(sh->shaders, 0, sizeof(shader_t) * capacity);
    memset(sh->gen, 0, sizeof(u16) * capacity);
    memset(sh->used, 0, sizeof(b8) * capacity);

    sh->fl = freelist_create(arena, capacity);
    if (!sh->fl) return NULL;

    g_sh = sh;

    sh->bound_shader = INVALID_32;
    sh->current_shader = NULL;

    sh->default_shader = shader_create("shaders/default");
    if (sh->default_shader == 0)
    {
        LOG_ERROR("FAILED TO CREATE DEFAULT SHADER!");
        return NULL;
    }

    LOG_INFO("Shader System Init");
    return sh;
}

void shader_sys_kill(shader_system_t *sh)
{
    if (!sh) return;

    shader_destroy(sh->default_shader);

    for (u32 i = 0; i < sh->capacity; i++)
    {
        if (sh->used[i] && sh->shaders[i].program)
        {
            render_destroy_shader(sh->shaders[i].program);
        }
        if (sh->shaders[i].uniforms)
        {
            FREE(sh->shaders[i].uniforms,
                 sizeof(shader_uniform_t) * sh->shaders[i].uniform_count,
                 MEM_SHADER);
        }
    }

    freelist_destroy(sh->fl);

    u64 total_size = sizeof(shader_t) * sh->capacity +
                     sizeof(u16) * sh->capacity + sizeof(b8) * sh->capacity;
    FREE(sh->shaders, total_size, MEM_SHADER);

    g_sh->bound_shader = INVALID_32;
    g_sh->current_shader = NULL;

    memset(sh, 0, sizeof(shader_system_t));
    LOG_INFO("Shader System Kill");
}

shader_handle_t shader_create(const char *name)
{
    if (!g_sh) return 0;

    u32 index = freelist_alloc(g_sh->fl);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free shader slot available");
        return 0;
    }
    LOG_DEBUG("Allocated shader at index %u (free: %u)", index,
              freelist_available_count(g_sh->fl));

    shader_t *s = &g_sh->shaders[index];
    memset(s, 0, sizeof(shader_t));

    strncpy(s->name, name, sizeof(s->name) - 1);

    g_sh->used[index] = true;
    g_sh->count++;

    u16 gen = g_sh->gen[index];
    shader_handle_t handle = handle_create((u16)index, gen);

    s->program = render_upload_shader(handle, name);
    render_cache_shader_uniform(s);

    return handle;
}

void shader_destroy(shader_handle_t handle)
{
    render_destroy_shader(handle);

    shader_t *s = shader_get(handle);
    if (!s) return;

    u16 index = handle_get_index(handle);

    if (s->uniforms)
    {
        FREE(s->uniforms, sizeof(shader_uniform_t) * s->uniform_count,
             MEM_SHADER);
    }

    // clear data
    memset(s, 0, sizeof(shader_t));

    freelist_free(g_sh->fl, index);

    g_sh->used[index] = false;
    g_sh->count--;

    g_sh->gen[index]++;
}

void shader_bind(shader_handle_t handle)
{
    if (g_sh->bound_shader == handle)
    {
        // LOG_DEBUG("shader_bind SKIPPED (already bound %u)", handle);
        return;
    }

    shader_t *s = shader_get(handle);
    if (!s || !s->program)
    {
        s = shader_get(g_sh->default_shader);
        if (!s || !s->program) return;

        handle = g_sh->default_shader;
    }
    LOG_DEBUG("shader_bind CALLED (new shader %u)", handle);

    render_bind_shader(s ? s->program : 0);
    g_sh->bound_shader = handle;
    g_sh->current_shader = s;
}

void shader_unbind(void)
{
    g_sh->bound_shader = INVALID_32;
    g_sh->current_shader = NULL;
    render_bind_shader(0);
}

shader_t *shader_get(shader_handle_t handle)
{
    if (!g_sh) return NULL;

    if (handle == 0) handle = g_sh->default_shader;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    if (index >= g_sh->capacity)
    {
        LOG_ERROR("Shader index %u out of bounds ", index);
        return NULL;
    }

    if (!g_sh->used[index]) return NULL;

    if (g_sh->gen[index] != generation) return NULL;

    shader_t *shader = &g_sh->shaders[index];
    if (!shader)
    {
        LOG_DEBUG("FUCK ME!!");
    }

    return shader;
}

void shader_set_model(shader_handle_t handle, mat4 m)
{
    render_set_model(handle, m);
}

void shader_set_lightpos(shader_handle_t handle, vec3 v)
{
    render_set_light_pos(handle, v);
}

void shader_set_lightcolor(shader_handle_t handle, vec3 v)
{
    render_set_light_color(handle, v);
}

void shader_set_object_color(shader_handle_t handle, vec3 v)
{
    render_set_object_color(handle, v);
}

void shader_set_viewpos(shader_handle_t handle, vec3 v)
{
    render_set_view_pos(handle, v);
}

void shader_set_sampler(shader_handle_t handle, i32 i)
{
    render_set_sampler(handle, i);
}

void shader_set_ambient(shader_handle_t handle, vec3 v)
{
    render_set_ambient(handle, v);
}

void shader_set_base_color(shader_handle_t handle, vec4 v)
{
    render_set_base_color(handle, v);
}

void shader_reset_state(void) { g_sh->bound_shader = INVALID_32; }
