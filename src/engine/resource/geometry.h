/**
 * @file geometry.h
 * @brief Geometry generation for basic 3D shapes
 *
 * Generates raw vertex data for simple 3D primitives. This is where the
 * triangles live! Only creates the geometry data - you still need to create
 * meshes from this.
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "engine/core/define.h"
#include "types.h"

/**
 * @brief Generate cube geometry data
 * @param width X-dimension size
 * @param height Y-dimension size
 * @param depth Z-dimension size
 * @return Geometry data with vertices and indices for a cube
 *
 * Creates a basic cube with positions, normals, and texture coordinates.
 * Each face has proper normals pointing outward.
 */
geometry_t geometry_gen_cube(f32 width, f32 height, f32 depth);

/**
 * @brief Generate plane geometry data
 * @param width Plane width
 * @param height Plane height
 * @param segments Number of subdivisions
 * @return Geometry data for a subdivided plane
 *
 * Creates a plane that can be subdivided for higher detail.
 * More segments = more triangles = smoother terrain (if you add height later).
 */
geometry_t geometry_gen_plane(f32 width, f32 height, u32 segments);

/**
 * @brief Destroy geometry data and free memory
 * @param geo Pointer to geometry data to destroy
 *
 * Cleans up the vertex and index arrays in the geometry.
 * Don't forget to call this when you're done with the geometry!
 */
void geometry_destroy(geometry_t *geo);

#endif // GEOMETRY_H
