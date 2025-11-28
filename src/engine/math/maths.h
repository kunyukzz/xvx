#ifndef MATHS_H
#define MATHS_H

#include "engine/core/define.h"
#include "math_types.h"
#include <math.h>

#define M_PI 3.14159265358979323846f
#define M_SQRT_2 1.41421356237309504880f
#define M_SQRT_3 1.73205080756887729252f
#define M_SEC2MS_MULTI 1000.0f
#define M_MS2SEC_MULTI 0.001f
#define M_INFINITE 1e30f
#define M_EPSILON 1.192092896e-07f

#define M_PI2 (2.0f * M_PI)
#define M_HALF_PI (0.5f * M_PI)
#define M_QUARTER_PI (0.25f * M_PI)
#define M_DEG2RAD (M_PI / 180.0f)
#define M_RAD2DEG (180.0f / M_PI)

INL b8 mat4_has_nan(const mat4 *m)
{
    for (u32 i = 0; i < 16; i++)
    {
        if (isnan(m->data[i])) return true;
    }
    return false;
}

#if DEBUG
INL void mat4_print_col(const char *name, mat4 m)
{
    LOG_DEBUG("%s:", name);
    for (int row = 0; row < 4; ++row)
    {
        LOG_DEBUG("[%.2f %.2f %.2f %.2f]",
                  m.data[0 * 4 + row],  // column 0, row
                  m.data[1 * 4 + row],  // column 1, row
                  m.data[2 * 4 + row],  // column 2, row
                  m.data[3 * 4 + row]); // column 3, row
    }
}
#else
INL void mat4_print_col(const char *name, const mat4 *m)
{
    (void)name;
    (void)m;
}
#endif

INL f32 m_sin(f32 x) { return sinf(x); }
INL f32 m_cos(f32 x) { return cosf(x); }
INL f32 m_tan(f32 x) { return tanf(x); }
INL f32 m_asin(f32 x) { return asinf(x); }
INL f32 m_acos(f32 x) { return acosf(x); }
INL f32 m_atan2(f32 y, f32 x) { return atan2f(y, x); }
INL f32 m_sqrt(f32 x) { return sqrtf(x); }
INL f32 m_abs(f32 x) { return fabsf(x); }
INL f32 m_floor(f32 x) { return floorf(x); }
INL f32 m_ceil(f32 x) { return ceilf(x); }
INL f32 m_pow(f32 base, f32 exp) { return powf(base, exp); }
INL f32 m_exp(f32 x) { return expf(x); }
INL f32 m_log(f32 x) { return logf(x); }

i32 m_random(void);
i32 m_random_in_range(i32 min, i32 max);
f32 m_frandom(void);
f32 m_frandom_in_range(f32 min, f32 max);

INL b8 m_power_of_2(u64 value)
{
    return (value != 0) && ((value & (value - 1)) == 0);
}

INL f32 m_deg_to_rad(f32 degree) { return degree * M_DEG2RAD; }
INL f32 m_rad_to_deg(f32 rad) { return rad * M_RAD2DEG; }

/*************************
 * VECTOR 2
 *************************/
// Declaration (implement in maths.c)
f32 vec2_length(vec2 v);

// Constructor
INL vec2 vec2_create(f32 x, f32 y) { return (vec2){.x = x, .y = y}; }
INL vec2 vec2_zero(void) { return (vec2){.x = 0.0f, .y = 0.0f}; }
INL vec2 vec2_one(void) { return (vec2){.x = 1.0f, .y = 1.0f}; }
INL vec2 vec2_up(void) { return vec2_create(0.0f, 1.0f); }
INL vec2 vec2_down(void) { return vec2_create(0.0f, -1.0f); }
INL vec2 vec2_left(void) { return vec2_create(-1.0f, 0.0f); }
INL vec2 vec2_right(void) { return vec2_create(1.0f, 0.0f); }

// Arithmetic
INL vec2 vec2_add(vec2 a, vec2 b) { return vec2_create(a.x + b.x, a.y + b.y); }
INL vec2 vec2_sub(vec2 a, vec2 b) { return vec2_create(a.x - b.x, a.y - b.y); }
INL vec2 vec2_mul(vec2 a, vec2 b) { return vec2_create(a.x * b.x, a.y * b.y); }
INL vec2 vec2_div(vec2 a, vec2 b) { return vec2_create(a.x / b.x, a.y / b.y); }

// Scalar
INL vec2 vec2_scale(vec2 v, f32 s) { return vec2_create(v.x * s, v.y * s); }
INL vec2 vec2_scale_div(vec2 v, f32 s)
{
    return vec2_create(v.x / s, v.y / s);
}

// Operations
INL f32 vec2_length_sq(vec2 v) { return v.x * v.x + v.y * v.y; }
INL f32 vec2_dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }
INL f32 vec2_cross(vec2 a, vec2 b) { return a.x * b.y - a.y * b.x; }

// Utility functions
INL f32 vec2_distance(vec2 a, vec2 b) { return vec2_length(vec2_sub(a, b)); }

INL f32 vec2_distance_sq(vec2 a, vec2 b)
{
    vec2 diff = vec2_sub(a, b);
    return vec2_length_sq(diff);
}

INL vec2 vec2_normalize(vec2 v)
{
    f32 len = vec2_length(v);
    if (len > M_EPSILON)
    {
        return vec2_scale(v, 1.0f / len);
    }
    return vec2_zero();
}

INL vec2 vec2_lerp(vec2 a, vec2 b, f32 t)
{
    return vec2_create(LERP(a.x, b.x, t), LERP(a.y, b.y, t));
}

INL b8 vec2_compare(vec2 a, vec2 b, f32 epsilon)
{
    return (ABS(a.x - b.x) <= epsilon) && (ABS(a.y - b.y) <= epsilon);
}

INL b8 vec2_is_zero(vec2 v) { return vec2_compare(v, vec2_zero(), M_EPSILON); }

/*************************
 * VECTOR 3
 *************************/
// Declaration (implement in maths.c)
f32 vec3_length(vec3 v);

// Constructor
INL vec3 vec3_create(f32 x, f32 y, f32 z)
{
    vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

INL vec3 vec3_zero(void) { return vec3_create(0.0f, 0.0f, 0.0f); }
INL vec3 vec3_one(void) { return vec3_create(1.0f, 1.0f, 1.0f); }
INL vec3 vec3_up(void) { return vec3_create(0.0f, 1.0f, 0.0f); }
INL vec3 vec3_down(void) { return vec3_create(0.0f, -1.0f, 0.0f); }
INL vec3 vec3_left(void) { return vec3_create(-1.0f, 0.0f, 0.0f); }
INL vec3 vec3_right(void) { return vec3_create(1.0f, 0.0f, 0.0f); }
INL vec3 vec3_forward(void) { return vec3_create(0.0f, 0.0f, -1.0f); }
INL vec3 vec3_back(void) { return vec3_create(0.0f, 0.0f, 1.0f); }

// Arithmetic
INL vec3 vec3_add(vec3 v1, vec3 v2)
{
    return vec3_create(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

INL vec3 vec3_sub(vec3 v1, vec3 v2)
{
    return vec3_create(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

INL vec3 vec3_mul(vec3 v1, vec3 v2)
{
    return vec3_create(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

INL vec3 vec3_div(vec3 v1, vec3 v2)
{
    return vec3_create(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

// Scalar
INL vec3 vec3_scale(vec3 v, f32 s)
{
    return (vec3){.x = v.x * s, .y = v.y * s, .z = v.z * s};
}

INL vec3 vec3_scale_div(vec3 v, f32 s)
{
    return (vec3){{v.x / s, v.y / s, v.z / s}};
}

// Operations
INL f32 vec3_length_sq(vec3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

INL f32 vec3_dot(vec3 v1, vec3 v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

INL vec3 vec3_cross(vec3 v1, vec3 v2)
{
    return vec3_create(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
                       v1.x * v2.y - v1.y * v2.x);
}

// Normalization
INL vec3 vec3_normalize(vec3 v)
{
    f32 len = vec3_length(v);
    if (len > M_EPSILON) return vec3_scale(v, 1.0f / len);
    return vec3_zero();
}

INL void vec3_normalize_ptr(vec3 *v)
{
    f32 len = vec3_length(*v);
    if (len > M_EPSILON)
    {
        v->x /= len;
        v->y /= len;
        v->z /= len;
    }
    else
    {
        *v = vec3_zero();
    }
}

// Utility
INL f32 vec3_distance(vec3 v1, vec3 v2)
{
    return vec3_length(vec3_sub(v1, v2));
}

INL f32 vec3_distance_sq(vec3 v1, vec3 v2)
{
    vec3 diff = vec3_sub(v1, v2);
    return vec3_length_sq(diff);
}

INL vec3 vec3_lerp(vec3 v1, vec3 v2, f32 t)
{
    return vec3_create(LERP(v1.x, v2.x, t), LERP(v1.y, v2.y, t),
                       LERP(v1.z, v2.z, t));
}

INL b8 vec3_compare(vec3 a, vec3 b, f32 tolerance)
{
    return (m_abs(a.x - b.x) <= tolerance) &&
           (m_abs(a.y - b.y) <= tolerance) && (m_abs(a.z - b.z) <= tolerance);
}

INL b8 vec3_is_zero(vec3 v) { return vec3_compare(v, vec3_zero(), M_EPSILON); }

INL vec3 vec3_reflect(vec3 v, vec3 normal)
{
    f32 dot = vec3_dot(v, normal);
    return vec3_sub(v, vec3_scale(normal, 2.0f * dot));
}

/*************************
 * VECTOR 4
 *************************/
// Declaration (implement in maths.c)
f32 vec4_length(vec4 v);

// Constructor
INL vec4 vec4_create(f32 x, f32 y, f32 z, f32 w)
{
    vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

INL vec4 vec4_unit_zero(void) { return vec4_create(0.0f, 0.0f, 0.0f, 0.0f); }
INL vec4 vec4_unit_one(void) { return vec4_create(1.0f, 1.0f, 1.0f, 1.0f); }
INL vec4 vec4_unit_x(void) { return vec4_create(1.0f, 0.0f, 0.0f, 0.0f); }
INL vec4 vec4_unit_y(void) { return vec4_create(0.0f, 1.0f, 0.0f, 0.0f); }
INL vec4 vec4_unit_z(void) { return vec4_create(0.0f, 0.0f, 1.0f, 0.0f); }
INL vec4 vec4_unit_w(void) { return vec4_create(0.0f, 0.0f, 0.0f, 1.0f); }

// Arithmetic
INL vec4 vec4_add(vec4 v1, vec4 v2)
{
    return vec4_create(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

INL vec4 vec4_sub(vec4 v1, vec4 v2)
{
    return vec4_create(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

INL vec4 vec4_mul(vec4 v1, vec4 v2)
{
    return vec4_create(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

INL vec4 vec4_div(vec4 v1, vec4 v2)
{
    return vec4_create(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

// Scalar
INL vec4 vec4_scale(vec4 v, f32 s)
{
    return (vec4){.x = v.x * s, .y = v.y * s, .z = v.z * s, .w = v.w * s};
}

INL vec4 vec4_scale_div(vec4 v, f32 s)
{
    return (vec4){{v.x / s, v.y / s, v.z / s, v.w / s}};
}

// Operations
INL f32 vec4_length_sq(vec4 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

INL f32 vec4_dot(vec4 v1, vec4 v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

// Normalization
INL vec4 vec4_normalize(vec4 v)
{
    f32 len = vec4_length(v);
    if (len > M_EPSILON) return vec4_scale(v, 1.0f / len);
    return vec4_unit_zero();
}

INL void vec4_normalize_ptr(vec4 *v)
{
    f32 len = vec4_length(*v);
    if (len > M_EPSILON)
    {
        v->x /= len;
        v->y /= len;
        v->z /= len;
        v->w /= len;
    }
    else
    {
        *v = vec4_unit_zero();
    }
}

// Utility
INL f32 vec4_distance(vec4 a, vec4 b) { return vec4_length(vec4_sub(a, b)); }

INL f32 vec4_distance_sq(vec4 a, vec4 b)
{
    vec4 diff = vec4_sub(a, b);
    return vec4_length_sq(diff);
}

INL vec4 vec4_lerp(vec4 a, vec4 b, f32 t)
{
    return vec4_create(LERP(a.x, b.x, t), LERP(a.y, b.y, t), LERP(a.z, b.z, t),
                       LERP(a.w, b.w, t));
}

INL b8 vec4_compare(vec4 a, vec4 b, f32 tolerance)
{
    return (m_abs(a.x - b.x) <= tolerance) &&
           (m_abs(a.y - b.y) <= tolerance) &&
           (m_abs(a.z - b.z) <= tolerance) && (m_abs(a.w - b.w) <= tolerance);
}

INL b8 vec4_is_zero(vec4 v)
{
    return vec4_compare(v, vec4_unit_zero(), M_EPSILON);
}

/*************************
 * MATRIX
 *************************/
// Declaration (implement in maths.c)
mat4 mat4_look_at(vec3 eye, vec3 target, vec3 up);
mat4 mat4_perspective(f32 fov, f32 aspect, f32 near, f32 far);
mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near,
                       f32 far);

// clang-format off
INL mat4 mat4_create(f32 m00, f32 m10, f32 m20, f32 m30,
                     f32 m01, f32 m11, f32 m21, f32 m31,
                     f32 m02, f32 m12, f32 m22, f32 m32,
                     f32 m03, f32 m13, f32 m23, f32 m33)
{
    mat4 m;
    m.m00 = m00; m.m10 = m10; m.m20 = m20; m.m30 = m30;
    m.m01 = m01; m.m11 = m11; m.m21 = m21; m.m31 = m31;
    m.m02 = m02; m.m12 = m12; m.m22 = m22; m.m32 = m32;
    m.m03 = m03; m.m13 = m13; m.m23 = m23; m.m33 = m33;
    return m;
}

INL mat4 mat4_identity(void)
{
    return mat4_create(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

INL mat4 mat4_zero(void)
{
    mat4 m;
    for (u32 i = 0; i < 16; i++) m.data[i] = 0.0f;
    return m;
}

INL vec3 mat4_forward(mat4 m) {
    return vec3_normalize(vec3_create(-m.data[8], -m.data[9], -m.data[10]));
}

INL vec3 mat4_backward(mat4 m) {
    return vec3_normalize(vec3_create(m.data[8], m.data[9], m.data[10]));
}

INL vec3 mat4_left(mat4 m) {
    return vec3_normalize(vec3_create(-m.data[0], -m.data[1], -m.data[2]));
}

INL vec3 mat4_right(mat4 m) {
    return vec3_normalize(vec3_create(m.data[0], m.data[1], m.data[2]));
}

INL vec3 mat4_up(mat4 m) {
    return vec3_normalize(vec3_create(m.data[4], m.data[5], m.data[6]));
}

INL vec3 mat4_down(mat4 m) {
    return vec3_normalize(vec3_create(-m.data[4], -m.data[5], -m.data[6]));
}

// Matrix multiplication (Column-major: A * B)
INL mat4 mat4_mul(mat4 m1, mat4 m2)
{
    mat4 result = mat4_zero();

    for (u32 row = 0; row < 4; row++) {
        for (u32 col = 0; col < 4; col++) {
			result.data[row * 4 + col] = 
		 		m1.data[row * 4 + 0] * m2.data[0 * 4 + col] +
		 		m1.data[row * 4 + 1] * m2.data[1 * 4 + col] +
		 		m1.data[row * 4 + 2] * m2.data[2 * 4 + col] +
		 		m1.data[row * 4 + 3] * m2.data[3 * 4 + col];
        }
    }
    return result;
}

INL vec4 mat4_mul_vec4(mat4 m, vec4 v) {
    return (vec4){{
        m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w,
        m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w,
        m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w,
        m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w 
    }};
}

INL vec3 mat4_mul_vec3(mat4 m, vec3 v)
{
    vec4 v4 = vec4_create(v.x, v.y, v.z, 1.0f);
    vec4 result = mat4_mul_vec4(m, v4);
    return vec3_create(result.x, result.y, result.z);
}

INL mat4 mat4_transpose(mat4 m)
{
    return mat4_create(
        m.m00, m.m01, m.m02, m.m03,  // Row 0 becomes Column 0
        m.m10, m.m11, m.m12, m.m13,  // Row 1 becomes Column 1
        m.m20, m.m21, m.m22, m.m23,  // Row 2 becomes Column 2
        m.m30, m.m31, m.m32, m.m33   // Row 3 becomes Column 3
    );
}

// Transformation matrices
INL mat4 mat4_translate(vec3 translation)
{
    mat4 m = mat4_identity();
    m.m03 = translation.x;
    m.m13 = translation.y; 
    m.m23 = translation.z;
    return m;
}

INL mat4 mat4_scale(vec3 scale)
{
    return mat4_create(
        scale.x, 0.0f,    0.0f,    0.0f,
        0.0f,    scale.y, 0.0f,    0.0f,
        0.0f,    0.0f,    scale.z, 0.0f,
        0.0f,    0.0f,    0.0f,    1.0f
    );
}

INL mat4 mat4_rotation_x(f32 angle)
{
    f32 c = m_cos(angle);
    f32 s = m_sin(angle);
    return mat4_create(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, c,    s,    0.0f,
        0.0f, -s,   c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

INL mat4 mat4_rotation_y(f32 angle)
{
    f32 c = m_cos(angle);
    f32 s = m_sin(angle);
    return mat4_create(
        c,    0.0f, -s,   0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        s,    0.0f, c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

INL mat4 mat4_rotation_z(f32 angle)
{
    f32 c = m_cos(angle);
    f32 s = m_sin(angle);
    return mat4_create(
        c,    s,    0.0f, 0.0f,
        -s,   c,    0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

INL mat4 mat4_rotation_xyz(vec3 angles)
{
    f32 cx = m_cos(angles.x);
    f32 sx = m_sin(angles.x);
    f32 cy = m_cos(angles.y);
    f32 sy = m_sin(angles.y);
    f32 cz = m_cos(angles.z);
    f32 sz = m_sin(angles.z);

    return mat4_create(
        cy * cz,       			 cy * sz,       		  -sy,      0.0f,
        cz * sx * sy - cx * sz,  cx * cz + sx * sy * sz,  cy * sx,  0.0f,
        cx * cz * sy + sx * sz,  cx * sy * sz - cz * sx,  cx * cy,  0.0f,
        0.0f,                    0.0f,                    0.0f,     1.0f
    );
}


INL mat4 mat4_rotation_xyz_angles(f32 x_rad, f32 y_rad, f32 z_rad)
{
    return mat4_rotation_xyz(vec3_create(x_rad, y_rad, z_rad));
}

INL mat4 mat4_inverse_rigid(mat4 m) {
    mat4 result = {0};
    
    // Transpose 3x3 rotation (columns become rows)
    result.m00 = m.m00; result.m10 = m.m01; result.m20 = m.m02;  // Row 0
    result.m01 = m.m10; result.m11 = m.m11; result.m21 = m.m12;  // Row 1
    result.m02 = m.m20; result.m12 = m.m21; result.m22 = m.m22;  // Row 2
    
    // Inverse translation: -Rᵀ * T
    float tx = m.m03, ty = m.m13, tz = m.m23;
    result.m03 = -(result.m00 * tx + result.m01 * ty + result.m02 * tz);
    result.m13 = -(result.m10 * tx + result.m11 * ty + result.m12 * tz);
    result.m23 = -(result.m20 * tx + result.m21 * ty + result.m22 * tz);
    
    result.m33 = 1.0f;

    return result;
}

INL mat4 mat4_transform(vec3 pos, vec3 rot, vec3 scale)
{
    mat4 trans_mat = mat4_translate(pos);
    mat4 rot_mat = mat4_rotation_xyz(rot);
    mat4 scale_mat = mat4_scale(scale);
    return mat4_mul(scale_mat, mat4_mul(rot_mat, trans_mat));
}

#endif // MATHS_H
