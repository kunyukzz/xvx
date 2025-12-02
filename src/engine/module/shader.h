/**
 * @file shader.h
 * @brief Shader system for managing GLSL shaders
 *
 * Handles shader compilation, linking, and uniform management.
 * This is where the magic happens - turning math into pretty pictures!
 */

#ifndef SHADER_H
#define SHADER_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/core/container/freelist.h"
#include "engine/math/math_types.h"
#include "engine/resource/types.h"

/**
 * @struct shader_system_t
 * @brief Internal shader system state
 *
 * Same object pool pattern as everything else because consistency is nice.
 * Has a default shader so things don't explode when no shader is set.
 */
typedef struct {
    arena_alloc_t *arena;
    u32 capacity;
    u32 count;

    shader_t *shaders;
    u16 *gen;
    b8 *used;

    freelist_t *fl;

    shader_handle_t default_shader;

    shader_handle_t bound_shader;
    shader_t *current_shader;
} shader_system_t;

/**
 * @brief Initialize the shader system
 * @param arena Memory arena for allocations
 * @param capacity Maximum number of shaders
 * @return Pointer to shader system, or NULL if failed
 *
 * Sets up the shader system and creates a default shader.
 */
shader_system_t *shader_sys_init(arena_alloc_t *arena, u32 capacity);

/**
 * @brief Shutdown the shader system
 * @param sh Pointer to shader system
 *
 * Deletes all shaders and frees GPU resources.
 */
void shader_sys_kill(shader_system_t *sh);

/**
 * @brief Create a shader from source files
 * @param name Base name for the shader (looks for name.vert and name.frag)
 * @return Handle to the created shader
 *
 * Compiles vertex and fragment shaders and links them into a program.
 * If compilation fails, returns an invalid handle and logs errors.
 */
shader_handle_t shader_create(const char *name);

/**
 * @brief Destroy a shader
 * @param handle Handle to the shader to destroy
 *
 * Deletes the shader program and frees its slot.
 * Don't destroy the default shader or things might get weird!
 */
void shader_destroy(shader_handle_t handle);

/**
 * @brief Bind a shader for rendering
 * @param handle Handle to the shader to bind
 *
 * Makes this shader active for all subsequent draw calls.
 * If handle is invalid, binds the default shader instead.
 */
void shader_bind(shader_handle_t handle);

/**
 * @brief Unbind any currently bound shader
 *
 * Stops using the current shader. Probably not necessary but feels right.
 */
void shader_unbind(void);

/**
 * @brief Get shader data from a handle
 * @param handle Handle to the shader
 * @return Pointer to shader data, or NULL if invalid
 *
 * Useful for setting uniforms or checking shader properties.
 */
shader_t *shader_get(shader_handle_t handle);

/**
 * @brief Helper set a mat4 uniform in the shader
 * @param handle Handle to the shader
 * @param m Matrix to set
 *
 * Updates a mat4 uniform in the currently bound shader.
 * The uniform name is probably hardcoded because I'm lazy.
 */
void shader_set_model(shader_handle_t handle, mat4 m);

void shader_set_lightpos(shader_handle_t handle, vec3 v);

void shader_set_lightcolor(shader_handle_t handle, vec3 v);

void shader_set_object_color(shader_handle_t handle, vec3 v);

void shader_set_viewpos(shader_handle_t handle, vec3 v);

void shader_set_sampler(shader_handle_t handle, i32 i);

void shader_set_ambient(shader_handle_t handle, vec3 v);

void shader_set_base_color(shader_handle_t handle, vec4 v);

void shader_reset_state(void);

#endif // SHADER_H
