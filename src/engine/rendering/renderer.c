#include "renderer.h"
#include "engine/core/memory.h"
#include "engine/resource/loader.h"
#include "engine/module/shader.h"
#include "engine/module/mesh.h"
#include "engine/module/texture.h"
#include "engine/module/handle_util.h"
#include "deps/glad/glad.h"
#include "engine/platform/window.h"

#include <string.h>

#define RENDER_MAX_MESH 512
#define RENDER_MAX_TEXTURE 512
#define RENDER_MAX_SHADER 64
#define RENDER_MAX_CMD 512

static GLuint compile_shader(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        LOG_ERROR("Shader compile error: %s", info);
    }
    return shader;
}

static void set_bind_mesh(render_system_t *rs, mesh_handle_t handle)
{
    u16 index = handle_get_index(handle);
    render_mesh_t *mesh = &rs->rs_mesh[index];

    glBindVertexArray(mesh->vao);
}

static void set_draw_bound(render_system_t *rs, mesh_handle_t handle)
{
    u16 index = handle_get_index(handle);
    glDrawElements(GL_TRIANGLES, (int)rs->rs_mesh[index].index_count,
                   GL_UNSIGNED_INT, 0);
}

static void update_world_uniform_buffer(render_system_t *rs)
{
    glBindBuffer(GL_UNIFORM_BUFFER, rs->world_ubo);

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4),
                    rs->cam->world.proj.data);

    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4),
                    rs->cam->world.view.data);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void update_ui_uniform_buffer(render_system_t *rs)
{
    glBindBuffer(GL_UNIFORM_BUFFER, rs->ui_ubo);

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4),
                    rs->cam->world.proj.data);

    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4),
                    rs->cam->world.view.data);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static render_system_t *g_rs = NULL;

render_system_t *render_sys_init(arena_alloc_t *arena)
{
    render_system_t *rs = arena_alloc(arena, sizeof(render_system_t));
    if (!rs) return NULL;
    memset(rs, 0, sizeof(render_system_t));

    rs->arena = arena;
    rs->cam = get_camera_system();

    int width, height;
    window_sys_get_size(&width, &height);

    rs->width = width;
    rs->height = height;

    rs->rs_mesh = ALLOC(sizeof(render_mesh_t) * RENDER_MAX_MESH, MEM_RENDER);
    rs->rs_shader =
        ALLOC(sizeof(render_shader_t) * RENDER_MAX_SHADER, MEM_RENDER);
    rs->rs_ui_mesh =
        ALLOC(sizeof(render_mesh_t) * RENDER_MAX_MESH, MEM_RENDER);
    rs->rs_cmd = ALLOC(sizeof(render_cmd_t) * RENDER_MAX_CMD, MEM_RENDER);

    // glad setup
    int version_glad = gladLoadGL();
    if (version_glad == 0)
    {
        LOG_FATAL("Failed to initialize OpenGL context");
        return NULL;
    }

    rs->curr_mat = INVALID_32;
    rs->curr_mesh = INVALID_32;

    rs->current_fbo = 0;
    rs->clear_color = (vec4){{0.0f, 0.0f, 0.0f, 1.0f}};

    // set world renderpass
    rs->main_pass = (render_pass_t){.fbo = rs->main_fbo,
                                    .clear_color = rs->clear_color,
                                    .clear_mask = GL_COLOR_BUFFER_BIT |
                                                  GL_DEPTH_BUFFER_BIT};

    glViewport(0, 0, rs->width, rs->height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LESS);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // world
    glGenBuffers(1, &rs->world_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, rs->world_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(render_ubo_t), NULL,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, rs->world_ubo);

    g_rs = rs;
    LOG_TRACE("Renderer: %s", glGetString(GL_RENDERER));
    LOG_INFO("Render System Init");
    return rs;
}

void render_sys_kill(render_system_t *rs)
{
    if (!rs) return;

    glDeleteVertexArrays(1, &rs->rs_ui_mesh->vao);
    glDeleteBuffers(1, &rs->rs_ui_mesh->vbo);
    glDeleteBuffers(1, &rs->rs_ui_mesh->ebo);

    glDeleteVertexArrays(1, &rs->rs_mesh->vao);
    glDeleteBuffers(1, &rs->rs_mesh->vbo);
    glDeleteBuffers(1, &rs->rs_mesh->ebo);

    FREE(rs->rs_cmd, sizeof(render_cmd_t), MEM_RENDER);

    FREE(rs->rs_ui_mesh, sizeof(render_mesh_t), MEM_RENDER);
    FREE(rs->rs_shader, sizeof(render_shader_t), MEM_RENDER);
    FREE(rs->rs_mesh, sizeof(render_mesh_t), MEM_RENDER);

    memset(rs, 0, sizeof(render_system_t));
    LOG_INFO("Render System Kill");
}

void render_sys_begin(render_system_t *rs, u8 id)
{
    update_world_uniform_buffer(rs);

    render_pass_t *pass = 0;
    u32 framebuffer = 0;
    switch (id)
    {
    case WORLD_PASS:
        pass = &rs->main_pass;
        framebuffer = rs->main_fbo;
        break;
    case DEBUG_UI_PASS:
        pass = &rs->debug_ui_pass;
        framebuffer = rs->ui_fbo;
        break;
    }

    if (rs->current_fbo != pass->fbo)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pass->fbo);
        rs->current_fbo = pass->fbo;
    }

    glClearColor(pass->clear_color.r, pass->clear_color.g, pass->clear_color.b,
                 pass->clear_color.a);
    glClear(pass->clear_mask);
}

void render_sys_end(render_system_t *rs, u8 id)
{
    (void)rs;
    (void)id;
}

void render_draw(render_system_t *rs, mesh_handle_t handle)
{
    u16 index = handle_get_index(handle);
    render_mesh_t *mesh = &rs->rs_mesh[index];
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, (int)rs->rs_mesh[index].index_count,
                   GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void render_push(render_system_t *rs, mesh_handle_t mesh)
{
    if (rs->cmd_count >= RENDER_MAX_CMD) return;

    rs->rs_cmd[rs->cmd_count++] = (render_cmd_t){.mesh = mesh};
}

void render_flush(render_system_t *rs)
{
    for (u32 i = 0; i < rs->cmd_count; ++i)
    {
        render_cmd_t *cmd = &rs->rs_cmd[i];

        if (cmd->mesh != rs->curr_mesh)
        {
            rs->curr_mesh = cmd->mesh;
            printf("change mesh\n");
            set_bind_mesh(rs, rs->curr_mesh);
        }

        set_draw_bound(rs, cmd->mesh);
    }
    rs->cmd_count = 0;
}

u32 render_upload_shader(shader_handle_t handle, const char *name)
{
    shader_t *shader = shader_get(handle);
    if (!shader)
    {
        LOG_ERROR("Mesh not found for handle: %u", handle);
        return 0;
    }

    u16 index = handle_get_index(handle);
    if (index >= RENDER_MAX_SHADER)
    {
        LOG_ERROR("Renderer mesh index out of bounds: %u (handle: %u)", index,
                  handle);
        return 0;
    }

    render_shader_t *rsh = &g_rs->rs_shader[index];

    // destroy old program if exists
    if (rsh->program != 0)
    {
        glDeleteProgram(rsh->program);
        rsh->program = 0;
    }

    char vert_path[MAX_PATH];
    char frag_path[MAX_PATH];
    snprintf(vert_path, sizeof(vert_path), "%s.vert.glsl", name);
    snprintf(frag_path, sizeof(frag_path), "%s.frag.glsl", name);

    u64 vert_size = 0, frag_size = 0;
    void *vert_src = read_file_text(vert_path, &vert_size);
    void *frag_src = read_file_text(frag_path, &frag_size);

    if (!vert_src || !frag_src)
    {
        LOG_ERROR("Failed to load shader files: %s", name);
        if (vert_src) FREE(vert_src, vert_size, MEM_RESOURCE);
        if (frag_src) FREE(frag_src, frag_size, MEM_RESOURCE);
        return 0;
    }

    GLuint vert = compile_shader(GL_VERTEX_SHADER, vert_src);
    GLuint frag = compile_shader(GL_FRAGMENT_SHADER, frag_src);

    if (!vert || !frag)
    {
        LOG_ERROR("Failed to compile shaders: %s", name);
        FREE(vert_src, vert_size, MEM_RESOURCE);
        FREE(frag_src, frag_size, MEM_RESOURCE);

        if (vert) glDeleteShader(vert);
        if (frag) glDeleteShader(frag);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    // Check link status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetProgramInfoLog(program, 512, NULL, info);
        LOG_ERROR("Program link error: %s", info);

        glDeleteProgram(program);
        glDeleteShader(vert);
        glDeleteShader(frag);
        FREE(vert_src, vert_size, MEM_RESOURCE);
        FREE(frag_src, frag_size, MEM_RESOURCE);
        return 0;
    }

    rsh->program = program;

    FREE(vert_src, vert_size, MEM_RESOURCE);
    FREE(frag_src, frag_size, MEM_RESOURCE);
    glDeleteShader(vert);
    glDeleteShader(frag);

    LOG_DEBUG("Uploaded shader: handle=%u index=%u program=%u", handle, index,
              program);

    return program;
}

void render_upload_mesh(mesh_handle_t handle, geometry_t *geo)
{
    mesh_t *mesh = mesh_get(handle);
    if (!mesh)
    {
        LOG_ERROR("Mesh not found for handle: %u", handle);
        return;
    }

    u16 index = handle_get_index(handle);
    if (index >= RENDER_MAX_MESH)
    {
        LOG_ERROR("Renderer mesh index out of bounds: %u (handle: %u)", index,
                  handle);
        return;
    }

    render_mesh_t *rmesh = &g_rs->rs_mesh[index];

    // Clean up existing GPU data if any
    if (rmesh->vao != 0)
    {
        glDeleteVertexArrays(1, &rmesh->vao);
        glDeleteBuffers(1, &rmesh->vbo);
        glDeleteBuffers(1, &rmesh->ebo);
    }

    rmesh->index_count = geo->indices_count;

    // TODO: change this to use monolithic buffer
    glGenVertexArrays(1, &rmesh->vao);
    glBindVertexArray(rmesh->vao);

    glGenBuffers(1, &rmesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rmesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, geo->vert_size * geo->vert_count,
                 geo->vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &rmesh->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rmesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 geo->indices_size * geo->indices_count, geo->indices,
                 GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (int)geo->vert_size,
                          (void *)OFFSETOF(vertex, position));

    // normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (int)geo->vert_size,
                          (void *)OFFSETOF(vertex, normal));

    // uv attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (int)geo->vert_size,
                          (void *)OFFSETOF(vertex, texcoord));

    glBindVertexArray(0);

    LOG_DEBUG("Uploaded mesh to GPU: handle=%u, index=%u, indices=%u", handle,
              index, geo->indices_count);
}

void render_upload_texture(texture_handle_t handle, void *pixel)
{
    if (!pixel) return;

    texture_t *t = texture_get(handle);
    if (!t) return;

    u16 index = handle_get_index(handle);
    if (index >= RENDER_MAX_TEXTURE)
    {
        LOG_ERROR("Renderer texture index out of bounds: %u (handle: %u)",
                  index, handle);
        return;
    }

    render_texture_t *rtex = &g_rs->rs_tex[index];

    glGenTextures(1, &rtex->id);
    if (rtex->id == 0)
    {
        LOG_ERROR("Failed to generate texture ID");
        return;
    }

    glBindTexture(GL_TEXTURE_2D, rtex->id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = t->channels == 4 ? GL_RGBA : GL_RGB;
    GLenum internal_format = t->channels == 4 ? GL_RGBA8 : GL_RGB8;

    glTexImage2D(GL_TEXTURE_2D, 0, (int)internal_format, (int)t->width,
                 (int)t->height, 0, format, GL_UNSIGNED_BYTE, pixel);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        LOG_ERROR("OpenGL error during texture upload: 0x%x", error);
        glDeleteTextures(1, &rtex->id);
        rtex->id = 0;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    t->internal = (void *)(uintptr_t)rtex->id;
    LOG_DEBUG("Uploaded texture to GPU: %s (ID: %u, %ux%u)", t->name, rtex->id,
              t->width, t->height);
}
