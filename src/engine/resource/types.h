/**
 * @file types.h (resource/types.h)
 * @brief Resource type definitions for the engine
 *
 * Contains all the basic resource types used throughout the engine.
 * Handles are just indices with generation counters for safety.
 * The actual data structures for meshes, textures, materials, and shaders.
 */

#ifndef RESOURCES_TYPE_H
#define RESOURCES_TYPE_H

#include "engine/core/define.h"
#include "engine/math/math_types.h"

// Handle types - just typedefs for now, but they pack index + generation
typedef u32 mesh_handle_t;
typedef u32 texture_handle_t;
typedef u32 material_handle_t;
typedef u32 shader_handle_t;
typedef u32 light_handle_t;

/**
 * @struct geometry_t
 * @brief Raw geometry data (vertices and indices)
 *
 * Contains vertex and index data before it's uploaded to the GPU.
 * This is the CPU-side representation of 3D geometry.
 */
typedef struct {
    void *vertices;
    u32 vert_count;
    u32 vert_size;

    void *indices;
    u32 indices_count;
    u32 indices_size;
} geometry_t;

/**
 * @struct mesh_t
 * @brief Mesh resource containing geometry and GPU state
 *
 * A mesh has geometry data and tracks whether it's been uploaded to GPU.
 * The internal pointer is for renderer-specific data (like VBO/VAO in OpenGL).
 */
typedef struct {
    geometry_t geometry;
    b8 uploaded;

    // pointing to internal renderer
    void *internal;
} mesh_t;

/**
 * @struct texture_t
 * @brief Texture resource containing image data and GPU state
 *
 * Stores texture metadata and a pointer to renderer-specific texture object.
 * The internal pointer is for OpenGL texture IDs or similar.
 */
typedef struct {
    u32 width;
    u32 height;
    u32 channels;
    char name[64];

    // pointing to internal renderer
    void *internal;
} texture_t;

/**
 * @struct material_t
 * @brief Material resource defining surface properties
 *
 * Basic PBR material parameters. Can have albedo and normal maps.
 * The actual shading happens in the shader using these parameters.
 */
typedef struct {
    char name[64];

    texture_handle_t albedo;
    texture_handle_t normal;

    vec4 base_color;
    f32 metallic;
    f32 roughness;
} material_t;

/**
 * @struct shader_uniform_t
 * @brief Shader uniform variable information
 *
 * Stores metadata about a uniform variable in a shader program.
 * Used for setting shader parameters at runtime.
 */
typedef struct {
    char name[64];
    i32 location;
    u32 type;
    i32 size;
} shader_uniform_t;

/**
 * @struct shader_t
 * @brief Shader program and its uniform information
 *
 * Contains a compiled shader program and information about its uniforms.
 * Has some hardcoded uniform locations for common variables (sorry!).
 */
typedef struct {
    char name[64];

    shader_uniform_t *uniforms;
    u32 uniform_count;

    u32 program;
    i32 model;

    i32 light_pos;
    i32 light_color;
    i32 object_color;
    i32 view_pos;

    i32 texture;
} shader_t;

typedef enum { LIGHT_DIRECTIONAL = 0, LIGHT_POINT, LIGHT_SPOT } light_type_t;

typedef struct {
    light_type_t type;

    // default properties
    vec3 position;
    vec3 color;
    f32 intensity;
    b8 enable;

    // Point/Spot light properties
    f32 radius;

    // Spot light properties
    f32 inner_cutoff;
    f32 outer_cutoff;
} light_t;

#endif // RESOURCES_TYPE_H
