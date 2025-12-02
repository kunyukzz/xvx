/**
 * @file mesh.h
 * @brief Mesh system for creating and managing 3D geometry
 *
 * Handles mesh creation, storage, and rendering. Currently has some basic
 * primitives because I'm too lazy to write a proper model loader.
 * The system uses handles because pointers are scary when things get deleted.
 */

#ifndef MESH_H
#define MESH_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/core/container/freelist.h"
#include "engine/resource/types.h"

/**
 * @struct mesh_system_t
 * @brief mesh system state
 *
 * Implementation of the mesh system - a simple object pool with generational
 * handles to prevent use-after-free. Basically just a fancy array with some
 * bookkeeping.
 */
typedef struct mesh_system_t {
    arena_alloc_t *arena;

    mesh_t *meshes;
    u16 *gen;
    b8 *used;

    freelist_t *fl;
    u32 capacity;
    u32 count;

    u32 internal_id;
} mesh_system_t;

/**
 * @brief Initialize the mesh system
 * @param arena Memory arena for allocations
 * @param capacity Maximum number of meshes we can handle
 * @return Pointer to the mesh system, or NULL if failed
 *
 * Sets up the mesh system with a fixed capacity.
 */
mesh_system_t *mesh_sys_init(arena_alloc_t *arena, u32 capacity);

/**
 * @brief Shutdown the mesh system and clean up all meshes
 * @param ms Pointer to the mesh system
 *
 * Destroys all meshes and frees any allocated resources.
 */
void mesh_sys_kill(mesh_system_t *ms);

/**
 * @brief Create a plane mesh
 * @param width Width of the plane
 * @param height Height of the plane
 * @param segments Number of segments for tessellation
 * @return Handle to the created mesh
 *
 * Creates a subdivided plane. More segments = more triangles = slower
 * rendering!
 */
mesh_handle_t mesh_create_plane(f32 width, f32 height, u32 segments);

/**
 * @brief Create a cube mesh
 * @param width X-dimension size
 * @param height Y-dimension size
 * @param depth Z-dimension size
 * @return Handle to the created cube mesh
 *
 * Creates a basic cube because every 3D engine needs cubes.
 */
mesh_handle_t mesh_create_cube(f32 width, f32 height, f32 depth);

/**
 * @brief Destroy a mesh and free its resources
 * @param handle Handle to the mesh to destroy
 *
 * Removes a mesh from the system and frees its GPU resources.
 */
void mesh_destroy(mesh_handle_t handle);

/**
 * @brief Get the mesh data from a handle
 * @param handle Handle to the mesh
 * @return Pointer to the mesh data, or NULL if invalid handle
 *
 * Looks up the actual mesh data from a handle.
 * Return NULL if the handle is stale or invalid (so check for NULL!).
 */
mesh_t *mesh_get(mesh_handle_t handle);

#endif // MESH_H
