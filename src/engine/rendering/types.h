/**
 * @file types.h
 * @brief Renderer-specific type definitions
 *
 * OpenGL-specific types used by the renderer internally.
 * These are the actual GPU resources that get created from our abstract types.
 * Separated from resource types to keep OpenGL details contained.
 */

#ifndef RENDERER_TYPES_H
#define RENDERER_TYPES_H

#include "engine/core/define.h"
#include "engine/math/math_types.h"
#include "engine/resource/types.h"

/**
 * @enum render_layer_t
 * @brief Rendering layers for different types of objects
 *
 * Bitmask flags for organizing what gets rendered when.
 * WORLD_PASS for normal 3D objects, DEBUG_UI_PASS for debug overlays.
 */
typedef enum { WORLD_PASS = 0x01, DEBUG_UI_PASS = 0x02 } render_layer_t;

/**
 * @struct render_pass_t
 * @brief Render pass configuration
 *
 * Settings for a rendering pass including framebuffer and clear operations.
 */
typedef struct {
    u32 fbo;
    vec4 clear_color;
    u32 clear_mask;
} render_pass_t;

/**
 * @struct render_ubo_t
 * @brief Uniform buffer object for camera matrices
 *
 * Contains projection and view matrices for shaders.
 * Stored in a UBO for efficient GPU access.
 */
typedef struct {
    mat4 proj;
    mat4 view;
} render_ubo_t;

/**
 * @struct render_mesh_t
 * @brief OpenGL-specific mesh data
 *
 * Contains the actual OpenGL buffer objects for a mesh.
 * This is what gets created from our abstract mesh_t.
 */
typedef struct {
    u32 vao;
    u32 vbo;
    u32 ebo;
    u32 index_count;
} render_mesh_t;

/**
 * @struct render_texture_t
 * @brief OpenGL-specific texture data
 *
 * Just wraps an OpenGL texture ID.
 * This is what gets created from our abstract texture_t.
 */
typedef struct {
    u32 id;
} render_texture_t;

/**
 * @struct render_shader_t
 * @brief OpenGL-specific shader data
 *
 * Just wraps an OpenGL shader program ID.
 * This is what gets created from our abstract shader_t.
 */
typedef struct {
    u32 program;
} render_shader_t;

/**
 * @struct render_cmd_t
 * @brief Render command for the render queue
 *
 * Simple command structure telling the renderer what to draw.
 * TODO: Add material handle and other properties later.
 */
typedef struct {
    mesh_handle_t mesh;
    // material_handle_t material;
} render_cmd_t;

/* TODO: for monolithic buffer later
typedef struct {
    u32 vert_count;
    u32 vert_size;
    u32 vert_offset;

    u32 ind_count;
    u32 ind_size;
    u32 ind_offset;
} internal_geo_t;
*/

#endif // RENDERER_TYPES_H
