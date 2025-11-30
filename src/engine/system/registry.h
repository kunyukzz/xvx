/**
 * @file registry.h
 * @brief Entity registry for the ECS-like system
 *
 * Basic entity-component registry that stores entities and their components.
 * Uses dense arrays for components because cache locality is supposedly
 * important. This is my understanding of ECS - probably not how the pros do
 * it!
 */

#ifndef REGISTRY_H
#define REGISTRY_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/core/container/darray.h"
#include "engine/core/container/freelist.h"
#include "entity.h"
#include "component.h"

/**
 * @struct registry_t
 * @brief Main registry storing all entities and their components
 *
 * Uses component masks to track which components each entity has.
 * Each component type has its own dense array for better cache performance.
 * TODO: this should be opaque, so game cannot see it.
 */
typedef struct registry_t {
    arena_alloc_t *arena;
    u32 capacity;

    darray_t *component_mask;
    darray_t *transform;
    darray_t *model;
    darray_t *light;
    darray_t *material;

    freelist_t *fl;
} registry_t;

/**
 * @brief Initialize the entity registry
 * @param arena Memory arena for allocations
 * @param capacity Maximum number of entities
 * @return Pointer to registry, or NULL if failed
 *
 * Sets up the registry with the specified capacity.
 * Allocates arrays for components and initializes everything.
 */
registry_t *registry_sys_init(arena_alloc_t *arena, u32 capacity);

/**
 * @brief Shutdown the registry and clean up
 * @param reg Pointer to registry
 *
 * Destroys all entities and frees all component arrays.
 */
void registry_sys_kill(registry_t *reg);

/**
 * @brief Create a new entity
 * @param reg Pointer to registry
 * @return New entity ID
 *
 * Creates a new entity with no components.
 * Returns ENTITY_NULL if the registry is full.
 */
entity_id registry_create_entity(registry_t *reg);

/**
 * @brief Destroy an entity and its components
 * @param reg Pointer to registry
 * @param entity Entity to destroy
 *
 * Removes an entity and all its components from the registry.
 * The entity ID becomes invalid and can be reused later.
 */
void registry_destroy_entity(registry_t *reg, entity_id entity);

/**
 * @brief Check if an entity ID is valid
 * @param reg Pointer to registry
 * @param entity Entity to check
 * @return True if entity exists and is valid
 *
 * Verifies that an entity exists and hasn't been destroyed.
 * Useful for safety checks before accessing components.
 */
b8 registry_is_entity_valid(registry_t *reg, entity_id entity);

/**
 * @brief Add a transform component to an entity
 * @param reg Pointer to registry
 * @param entity Entity to add component to
 * @param transform Transform component data
 *
 * Adds or replaces the transform component for an entity.
 * Updates the component mask to include COMP_TRANSFORM.
 */
void registry_add_transform(registry_t *reg, entity_id entity,
                            transform_comp_t transform);

/**
 * @brief Add a model component to an entity
 * @param reg Pointer to registry
 * @param entity Entity to add component to
 * @param model Model component data
 *
 * Adds or replaces the model component for an entity.
 * Updates the component mask to include COMP_MODEL.
 */
void registry_add_model(registry_t *reg, entity_id entity, model_comp_t model);

/**
 * @brief Get transform component for an entity
 * @param reg Pointer to registry
 * @param entity Entity to get component from
 * @return Pointer to transform component, or NULL if entity doesn't have one
 *
 * Returns a pointer to the entity's transform component.
 * The pointer remains valid until the component is removed or entity is
 * destroyed.
 */
transform_comp_t *registry_get_transform(registry_t *reg, entity_id entity);

/**
 * @brief Check if entity has a transform component
 * @param reg Pointer to registry
 * @param entity Entity to check
 * @return True if entity has transform component
 */
b8 registry_has_transform(registry_t *reg, entity_id entity);

/**
 * @brief Get model component for an entity
 * @param reg Pointer to registry
 * @param entity Entity to get component from
 * @return Pointer to model component, or NULL if entity doesn't have one
 */
model_comp_t *registry_get_model(registry_t *reg, entity_id entity);

/**
 * @brief Check if entity has a model component
 * @param reg Pointer to registry
 * @param entity Entity to check
 * @return True if entity has model component
 */
b8 registry_has_model(registry_t *reg, entity_id entity);

/**
 * @brief Get the global registry system
 * @return Pointer to registry system
 *
 * Global accessor for the registry system.
 */
registry_t *get_registry_system(void);

#endif // REGISTRY_H
