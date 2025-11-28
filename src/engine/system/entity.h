/**
 * @file entity.h
 * @brief Entity ID system for the ECS-like thing I'm building
 *
 * Simple entity handles that are just IDs pointing to data elsewhere.
 * This is my attempt at data-oriented design, but I'm probably doing it wrong.
 * The generation counter is supposed to prevent use-after-free.
 */

#ifndef ENTITY_H
#define ENTITY_H

#include "engine/core/define.h"

/**
 * @struct entity_id
 * @brief Handle that identifies an entity in the world
 *
 * Just an index + generation, no actual data here.
 * All the component data lives in elsewhere (I think that's how ECS works?).
 * The generation helps detect when an entity has been destroyed and recreated.
 */
typedef struct {
    u32 index;
    u32 generation;
} entity_id;

/**
 * @def ENTITY_NULL
 * @brief Null entity constant for invalid/empty entities
 *
 * Use this when you don't have a real entity, or to initialize stuff.
 * Basically the entity equivalent of NULL.
 */
#define ENTITY_NULL ((entity_id){0, 0})

/**
 * @brief Get the index part of an entity ID
 * @param entity The entity ID to examine
 * @return The index (which array slot this entity lives in)
 *
 * Useful for actually looking up entity data in arrays.
 */
INL u32 entity_get_index(entity_id entity) { return entity.index; }

/**
 * @brief Get the generation part of an entity ID
 * @param entity The entity ID to examine
 * @return The generation counter
 *
 * Check this against the current generation to see if the entity is still
 * valid. If it doesn't match, entity was destroyed & something else might be
 * in its spot.
 */
INL u32 entity_get_generation(entity_id entity) { return entity.generation; }

/**
 * @brief Create a new entity ID
 * @param index Array index for this entity
 * @param generation Generation counter
 * @return A new entity ID
 *
 * Factory function for entity IDs. Mostly used internally by the registry.
 * If you're calling this from game code, you're probably doing something
 * wrong.
 */
INL entity_id entity_create(u32 index, u32 generation)
{
    return (entity_id){index, generation};
}

/**
 * @brief Check if two entity IDs are the same
 * @param a First entity ID
 * @param b Second entity ID
 * @return True if both index and generation match
 *
 * Entity equality check. Both index AND generation must match.
 * This is important because the same slot can be reused with different
 * generations.
 */
INL b8 entity_eq(entity_id a, entity_id b)
{
    return a.index == b.index && a.generation == b.generation;
}

/**
 * @brief Check if an entity ID is null/invalid
 * @param entity The entity ID to check
 * @return True if this is a null entity
 *
 * Useful for checking if an entity reference is valid before using it.
 * If this returns true, don't try to use this entity for anything!
 */
INL b8 entity_is_null(entity_id entity)
{
    return entity.index == 0 && entity.generation == 0;
}

#endif // ENTITY_H
