#include "maths.h"

f32 vec2_length(vec2 v) { return sqrtf(vec2_length_sq(v)); }

f32 vec3_length(vec3 v) { return sqrtf(vec3_length_sq(v)); }

f32 vec4_length(vec4 v) { return sqrtf(vec4_length_sq(v)); }

// clang-format off
mat4 mat4_look_at(vec3 eye, vec3 target, vec3 up)
{
    vec3 z = vec3_normalize(vec3_sub(target, eye));
    vec3 x = vec3_normalize(vec3_cross(z, up));
    vec3 y = vec3_cross(x, z);

    z = vec3_scale(z, -1.0f); // OpenGL looks down negative Z

    return mat4_create(x.x,   x.y,   x.z,  -vec3_dot(x, eye),
					   y.x,   y.y,   y.z,  -vec3_dot(y, eye),
					   z.x,   z.y,   z.z,  -vec3_dot(z, eye),
                       0.0f,  0.0f,  0.0f, 1.0f);
}

mat4 mat4_perspective(f32 fov_degrees, f32 aspect, f32 near, f32 far)
{
    f32 fov_rad = m_deg_to_rad(fov_degrees);
    f32 tan_half_fov = tanf(fov_rad / 2.0f);
    f32 f = 1.0f / tan_half_fov;
    f32 range_inv = 1.0f / (near - far);

    return mat4_create(
        f / aspect, 0.0f,   0.0f, 								0.0f,
        0.0f, 		f, 	    0.0f, 								0.0f,
        0.0f, 		0.0f,   (far + near) * range_inv, 			-1.0f,
        0.0f, 		0.0f,   (2.0f * far * near) * range_inv, 	0.0f
    );
}

mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near,
                       f32 far)
{
    f32 rl = 1.0f / (right - left);
    f32 tb = 1.0f / (top - bottom);
    f32 fn = 1.0f / (far - near);

    return mat4_create(2.0f * rl,  0.0f, 		0.0f, 			-(right + left) * rl,
					   0.0f, 	   2.0f * tb, 	0.0f, 			-(top + bottom) * tb,
					   0.0f,       0.0f, 		-2.0f * fn, 	-(far + near) * fn,
					   0.0f,       0.0f, 		0.0f, 			1.0f);
}
