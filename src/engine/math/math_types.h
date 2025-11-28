/**
 * @file math_types.h
 * @brief Basic math types for 3D graphics - vectors, matrices, and such
 *
 * My attempt at math types that don't completely suck. Has some SSE alignment
 * because I heard that makes things faster, but I'm not really sure how to use
 * it properly. The unions are there so I can access data in different ways
 * without casting everywhere.
 */

#ifndef MATH_TYPES_H
#define MATH_TYPES_H

#include "engine/core/define.h"

#if defined(__SSE__) || (_M_IX86_FP >= 1) || (_M_X64 == 1)
#    define MATH_SSE 1
#    include <xmmintrin.h> // SSE
#    include <emmintrin.h> // SSE2
#else
#    define MATH_SSE 0
#endif

/**
 * @union vec2
 * @brief 2-component vector for 2D stuff
 *
 * Useful for texture coordinates, 2D positions, or when you're feeling 2D.
 * The unions let me access components by different names depending on context.
 */
typedef union VEC2
{
    struct {
        f32 x, y;
    };
    struct {
        f32 u, v;
    };
    struct {
        f32 r, g;
    };
    f32 elements[2];
} ALIGN(8) vec2;

/**
 * @union vec3
 * @brief 3-component vector for 3D positions and colors
 *
 * The workhorse for 3D graphics. Used for positions, normals, colors, etc.
 * Probably the most used type in the entire engine.
 */
typedef union VEC3
{
    struct {
        f32 x, y, z;
    };
    struct {
        f32 r, g, b;
    };
    struct {
        f32 u, v, t;
    };
    f32 elements[3];
} ALIGN(16) vec3;

/**
 * @union vec4
 * @brief 4-component vector for homogenous coordinates and colors
 *
 * Used for positions in homogenous space (with w=1), RGBA colors,
 * or when you need an extra component for some reason.
 */
typedef union VEC4
{
    struct {
        f32 x, y, z, w;
    };
    struct {
        f32 r, g, b, a;
    };
    struct {
        f32 u, v, t, s;
    };
    f32 elements[4];
} ALIGN(16) vec4;

/**
 * @union mat4
 * @brief 4x4 matrix for transformations
 *
 * Used for model, view, and projection matrices. Can be accessed by:
 * - data[16] for flat array
 * - columns[4] for column-major (OpenGL style)
 * - individual elements m00..m33
 *
 * Yes, it's column-major because OpenGL likes it that way.
 */
typedef union MAT4
{
    f32 data[16];
    vec4 columns[4];

    struct {
        f32 m00, m10, m20, m30;
        f32 m01, m11, m21, m31;
        f32 m02, m12, m22, m32;
        f32 m03, m13, m23, m33;
    };
} ALIGN(16) mat4;

/**
 * @union quat
 * @brief Quaternion for rotations
 *
 * Used for smooth rotations without gimbal lock.
 * I don't fully understand quaternions yet.
 */
typedef union QUAT
{
    struct {
        f32 x, y, z, w;
    };

    vec4 as_vec4;
    f32 elements[4];
} ALIGN(16) quat;

/**
 * @struct vertex
 * @brief Vertex format for mesh data
 *
 * Basic vertex with position, normal, and texture coordinates.
 * Probably should add tangent/bitangent later for normal mapping.
 */
typedef struct vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
} ALIGN(16) vertex;

#endif // MATH_TYPES_H
