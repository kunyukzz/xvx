/**
 * @file renderer.h
 * @brief Main rendering system for OpenGL graphics
 *
 * The heart of the graphics pipeline - handles rendering passes,
 * GPU resource management, and drawing commands.
 * Manages framebuffers, shaders, meshes, and a simple render command queue.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "camera.h"
#include "types.h"

/**
 * @struct render_system_t
 * @brief Main rendering system state
 *
 * Contains all the OpenGL state, framebuffers, and render commands.
 * Manages multiple render passes and keeps track of current rendering state.
 * Probably too monolithic but it works for now!
 */
typedef struct {
    arena_alloc_t *arena;
    camera_system_t *cam;

    i32 width, height;

    render_pass_t main_pass;
    render_pass_t debug_ui_pass;

    u32 current_fbo;
    vec4 clear_color;

    u32 main_fbo;
    u32 ui_fbo;

    render_mesh_t *rs_mesh;
    render_mesh_t *rs_ui_mesh;
    render_texture_t *rs_tex;
    render_cmd_t *rs_cmd;
    u32 cmd_count;

    u32 world_ubo;
    u32 ui_ubo;

    render_shader_t *rs_shader;

    material_handle_t curr_mat;
    mesh_handle_t curr_mesh;
} render_system_t;

/**
 * @brief Initialize the rendering system
 * @param arena Memory arena for allocations
 * @return Pointer to render system, or NULL if failed
 *
 * Sets up OpenGL context, creates framebuffers, compiles shaders,
 * and initializes all renderer state. If this fails, check your OpenGL setup!
 */
render_system_t *render_sys_init(arena_alloc_t *arena);

/**
 * @brief Shutdown the rendering system
 * @param rs Pointer to render system
 *
 * Cleans up all OpenGL resources (buffers, textures, shaders, framebuffers).
 * All GPU resources become invalid after this!
 */
void render_sys_kill(render_system_t *rs);

/**
 * @brief Begin a render pass
 * @param rs Pointer to render system
 * @param id Render pass ID (WORLD_PASS or DEBUG_UI_PASS)
 *
 * Sets up the framebuffer and clear state for a render pass.
 * Call this before rendering objects for a specific pass.
 * TODO: maybe handle this internally?
 */
void render_sys_begin(render_system_t *rs, u8 id);

/**
 * @brief End a render pass
 * @param rs Pointer to render system
 * @param id Render pass ID
 *
 * Finalizes the render pass. Might do post-processing or blitting to screen.
 * TODO: maybe handle this internally?
 */
void render_sys_end(render_system_t *rs, u8 id);

/**
 * @brief Draw a mesh immediately
 * @param rs Pointer to render system
 * @param handle Handle to mesh to draw
 *
 * Renders a mesh immediately (bypasses command queue).
 * Useful for when you need to draw something right now!
 */
void render_draw(render_system_t *rs, mesh_handle_t handle);

/**
 * @brief Push a render command to the queue
 * @param rs Pointer to render system
 * @param mesh Handle to mesh to render
 *
 * Adds a draw command to the render queue for later processing.
 * More efficient than immediate drawing for multiple objects.
 * TODO: expose this to engine level, maybe?
 */
void render_push(render_system_t *rs, mesh_handle_t mesh);

/**
 * @brief Execute all queued render commands
 * @param rs Pointer to render system
 *
 * Processes the entire render command queue and draws everything.
 * Called at the end of the frame to render all queued objects.
 * TODO: expose this to engine level, maybe?
 */
void render_flush(render_system_t *rs);

/************************************
 * FUNCTION CALL FOR MODULE
 ************************************/

/**
 * @brief Upload and compile a shader to GPU
 * @param handle Shader handle to associate with
 * @param name Shader name for file lookup
 * @return OpenGL shader program ID, or 0 if failed
 *
 * Called by shader system to compile and link shaders.
 * Looks for name.vert and name.frag files in shader directory.
 */
u32 render_upload_shader(shader_handle_t handle, const char *name);

/**
 * @brief Upload mesh geometry to GPU buffers
 * @param handle Mesh handle to associate with
 * @param geo Geometry data to upload
 *
 * Called by mesh system to create VBO/VAO/EBO for a mesh.
 * The geometry data is copied to GPU memory.
 */
void render_upload_mesh(mesh_handle_t handle, geometry_t *geo);

/**
 * @brief Upload texture pixels to GPU
 * @param handle Texture handle to associate with
 * @param pixel Raw pixel data to upload
 *
 * Called by texture system to create OpenGL texture from pixel data.
 * Handles format conversion and mipmap generation.
 */
void render_upload_texture(texture_handle_t handle, void *pixel);

#endif // RENDERER_H
