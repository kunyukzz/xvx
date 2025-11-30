/**
 * @file component.h
 * @brief Component definitions for the ECS-like system
 *
 * Basic components for transform and model rendering.
 * This is my understanding of ECS - probably not how the pros do it!
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include "engine/core/define.h"
#include "engine/math/maths.h"
#include "engine/resource/types.h"

/**
 * @enum component_type_t
 * @brief Bitmask flags for component types
 *
 * Using bitflags so entities can have multiple components.
 * Or at least that's the plan - we'll see if I actually implement it properly!
 */
typedef enum {
    COMP_NONE = 0,
    COMP_TRANSFORM = 1 << 0,
    COMP_MODEL = 1 << 1,
    COMP_LIGHT = 1 << 2,
    COMP_MATERIAL = 1 << 3
} component_type_t;

/**
 * @struct transform_comp_t
 * @brief Transform component for entity position/orientation
 *
 * Stores position, rotation, scale and a cached matrix.
 * Has a dirty flag so we only recalculate the matrix when needed.
 * Probably should use quaternions for rotation but that's too hard right now.
 */
typedef struct {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    mat4 matrix;

    b8 dirty;
} transform_comp_t;

/**
 * @struct model_comp_t
 * @brief Model component for rendering
 *
 * Just holds handles to mesh and material resources.
 * The actual data lives elsewhere - this is just a reference.
 */
typedef struct {
    mesh_handle_t mesh;
    texture_handle_t texture; // TODO: remove this later
    material_handle_t material;
} model_comp_t;

/**
 * @struct light_comp_t
 * @brief Light component for rendering
 *
 * Just holds handles to light resources.
 * The actual data lives elsewhere - this is just a reference.
 */
typedef struct {
    light_handle_t handle;
    b8 enabled;
} light_comp_t;

/**
 * @struct material_comp_t
 * @brief Material component for rendering
 *
 * Just holds handles to material resources.
 * The actual data lives elsewhere - this is just a reference.
 */
typedef struct {
    material_handle_t handle;
} material_comp_t;

/**
 * @brief Create a default transform component
 * @return Transform component at origin, no rotation, unit scale
 *
 * Everything starts at the origin facing forward.
 * The dirty flag is set so the matrix gets calculated first time.
 */
INL transform_comp_t transform_comp_default(void)
{
    return (transform_comp_t){.position = vec3_zero(),
                              .rotation = vec3_zero(),
                              .scale = vec3_one(),
                              .matrix = mat4_identity(),
                              .dirty = true};
}

/**
 * @brief Create a model component
 * @param mesh Handle to mesh resource
 * @param material Handle to material resource
 * @return Model component ready for rendering
 *
 * Just bundles mesh and material together.
 * If either handle is invalid, it'll probably crash. Fun!
 */
INL model_comp_t model_comp_create(mesh_handle_t mesh, texture_handle_t tex,
                                   material_handle_t mat)
{
    return (model_comp_t){.mesh = mesh, .texture = tex, .material = mat};
}

#endif // COMPONENT_H
