/**
 * @file material.h
 * @brief Material system for surface properties and shading
 *
 * Handles material creation and management. Currently has basic PBR parameters
 * because that's what all the cool engines are doing these days.
 * I don't fully understand PBR but the math looks fancy!
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/core/container/freelist.h"
#include "engine/resource/types.h"

/**
 * @struct material_system_t
 * @brief material system state
 *
 * Same pattern as the mesh system - object pool with generational handles.
 */
typedef struct {
    arena_alloc_t *arena;
    u32 capacity;
    u32 count;

    material_t *materials;
    u16 *gen;
    b8 *used;

    freelist_t *fl;

    u32 internal_id;

    material_handle_t bound_material;
    // shader_handle_t bound_shader;
    texture_handle_t bound_albedo;
} material_system_t;

/**
 * @brief Initialize the material system
 * @param arena Memory arena for allocations
 * @param capacity Maximum number of materials
 * @return Pointer to the material system, or NULL if failed
 *
 * Sets up the material system with fixed capacity.
 */
material_system_t *material_sys_init(arena_alloc_t *arena, u32 capacity);

/**
 * @brief Shutdown the material system
 * @param ms Pointer to the material system
 *
 * Cleans up all materials and frees GPU resources.
 */
void material_sys_kill(material_system_t *ms);

/**
 * @brief Create a new material
 * @param name Descriptive name for the material
 * @param base_color Base color/tint of the material
 * @param albedo Handle to albedo texture
 * @param metallic How metallic the surface is (0-1)
 * @param roughness How rough the surface is (0-1)
 * @return Handle to the created material
 *
 * Creates a PBR material with the given parameters.
 * If you don't have a texture, pass an invalid handle and
 * it'll use the base color.
 *
 * TODO: base color should using vec3!!
 */
material_handle_t material_create(const char *name, vec4 base_color,
                                  texture_handle_t albedo, f32 metallic,
                                  f32 roughness);

/**
 * @brief Destroy a material
 * @param handle Handle to the material to destroy
 *
 * Removes a material and frees its resources.
 */
void material_destroy(material_handle_t handle);

/**
 * @brief Get material data from a handle
 * @param handle Handle to the material
 * @return Pointer to the material data, or NULL if invalid
 *
 * Looks up the actual material data from a handle.
 * Useful for updating material properties at runtime.
 */
material_t *material_get(material_handle_t handle);

/**
 * @brief Bind a material and all its associated resource
 * @param handle Material to bind
 *
 * Efficiently binds shader, textures, and sets uniforms only when needed.
 */
void material_bind(material_handle_t material);

void material_reset_state(void);

#endif // MATERIAL_H
