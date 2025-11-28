#include "shader.h"
#include "engine/core/memory.h"
#include "engine/rendering/renderer.h"
#include "handle_util.h"
#include "deps/glad/glad.h"

// std
#include <string.h>

static shader_system_t *g_sh = NULL;

static void shader_cache_uniforms(shader_t *s)
{
    u32 prog = s->program;

    s->model = glGetUniformLocation(prog, "model");
    s->light_pos = glGetUniformLocation(prog, "light_pos");
    s->view_pos = glGetUniformLocation(prog, "view_pos");
    s->light_color = glGetUniformLocation(prog, "light_color");
    s->object_color = glGetUniformLocation(prog, "object_color");

    LOG_DEBUG("Cached uniforms for '%s'", s->name);
}

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

    sh->default_shader = shader_create("shaders/test");
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

    for (u32 i = 0; i < sh->capacity; i++)
    {
        if (sh->used[i] && sh->shaders[i].program)
        {
            glDeleteProgram(sh->shaders[i].program);
        }
        if (sh->shaders[i].uniforms)
        {
            FREE(sh->shaders[i].uniforms,
                 sizeof(shader_uniform_t) * sh->shaders[i].uniform_count,
                 MEM_SHADER);
        }
    }

    shader_destroy(sh->default_shader);

    u64 total_size = sizeof(shader_t) * sh->capacity +
                     sizeof(u16) * sh->capacity + sizeof(b8) * sh->capacity;
    FREE(sh->shaders, total_size, MEM_SHADER);

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
    shader_cache_uniforms(s);

    return handle;
}

void shader_destroy(shader_handle_t handle)
{
    shader_t *s = shader_get(handle);
    if (!s) return;

    u16 index = handle_get_index(handle);

    if (s->program) glDeleteProgram(s->program);

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
    shader_t *s = shader_get(handle);
    if (!s || !s->program)
    {
        s = shader_get(g_sh->default_shader);
    }

    if (s && s->program)
    {
        glUseProgram(s->program);
    }
    else
    {
        glUseProgram(0);
    }
}

void shader_unbind(void) { glUseProgram(0); }

shader_t *shader_get(shader_handle_t handle)
{
    if (!g_sh) return NULL;

    if (handle == 0) handle = g_sh->default_shader;

    u16 index = handle_get_index(handle);
    u16 generation = handle_get_generation(handle);

    if (index >= g_sh->capacity)
    {
        LOG_ERROR("Index %u out of bounds ", index);
        return NULL;
    }

    if (index >= g_sh->capacity) return NULL;

    if (!g_sh->used[index]) return NULL;

    if (g_sh->gen[index] != generation) return NULL;

    shader_t *shader = &g_sh->shaders[index];
    if (!shader)
    {
        LOG_DEBUG("FUCK YOU!!");
    }

    return shader;
}

void shader_set_mat4(shader_handle_t handle, mat4 m)
{
    shader_t *s = shader_get(handle);
    if (!s) return;

    if (s->model == -1)
    {
        LOG_DEBUG("Shader %u has no model uniform location", handle);
        return;
    }
    glUniformMatrix4fv(s->model, 1, GL_FALSE, m.data);
}

void shader_set_vec3(shader_handle_t handle, i32 loc, vec3 v)
{
    (void)handle;
    glUniform3fv(loc, 1, &v.x);
}
