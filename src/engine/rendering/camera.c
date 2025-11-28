#include "camera.h"
#include "engine/platform/window.h"
#include "engine/math/maths.h"

// std
#include <string.h>

static void recalculate_matrix(camera_t *cam)
{
    if (!cam) return;

    mat4 rotation = mat4_rotation_xyz(cam->rotation);
    mat4 translation = mat4_translate(cam->position);
    mat4 view = mat4_mul(translation, rotation);

    cam->view = mat4_inverse_rigid(view);
    cam->dirty = false;
}

static void update_proj_pers(camera_t *cam, f32 aspect_ratio)
{
    if (!cam) return;

    cam->proj = mat4_perspective(cam->fov, aspect_ratio, cam->near, cam->far);
    cam->aspect_ratio = aspect_ratio;
}

/*
static void update_proj_ortho(camera_t *cam, f32 left, f32 right,
                              f32 bottom, f32 top)
{
    cam->proj =
        mat4_orthographic(left, right, bottom, top, cam->near, cam->far);
    cam->view = mat4_identity();
}
*/

static camera_system_t *g_cs = NULL;

camera_system_t *camera_sys_init(arena_alloc_t *arena)
{
    camera_system_t *cs = arena_alloc(arena, sizeof(camera_system_t));
    if (!cs)
    {
        LOG_ERROR("arena_alloc returned NULL!");
        return NULL;
    }
    memset(cs, 0, sizeof(camera_system_t));

    cs->arena = arena;
    cs->world.near = 0.1f;
    cs->world.far = 100.0f;
    cs->world.fov = 45.0f;
    cs->world.position = (vec3){{0.0f, 0.0f, 5.0f}};
    cs->world.rotation = (vec3){{m_deg_to_rad(-30.0f), 0.0f, 0.0f}};
    cs->world.proj_type = CAMERA_PROJECTION_PERSPECTIVE;
    cs->world.dirty = true;

    g_cs = cs;
    LOG_INFO("Camera System Init");
    return cs;
}

void camera_sys_kill(camera_system_t *cs)
{
    if (!cs) return;

    memset(cs, 0, sizeof(camera_system_t));
    LOG_INFO("Camera System Kill");
}

void camera_update(camera_system_t *cs)
{
    int width, height;
    window_sys_get_size(&width, &height);
    f32 aspect_ratio = (f32)width / (f32)height;

    if (cs->world.dirty || cs->world.aspect_ratio != aspect_ratio)
    {
        update_proj_pers(&cs->world, aspect_ratio);
        recalculate_matrix(&cs->world);
    }
}

camera_system_t *get_camera_system(void) { return g_cs; }

void cam_yaw(camera_system_t *cam, f32 amount)
{
    cam->world.rotation.y += amount;
    cam->world.dirty = true;
}

void cam_pitch(camera_system_t *cam, f32 amount)
{
    cam->world.rotation.x += amount;
    f32 limit = m_deg_to_rad(89.0f);
    cam->world.rotation.x = CLAMP(cam->world.rotation.x, -limit, limit);
    cam->world.dirty = true;
}

void cam_move_forward(camera_system_t *cam, vec3 *velocity, f32 amount)
{
    vec3 forward = mat4_forward(cam->world.view);
    *velocity = vec3_add(*velocity, vec3_scale(forward, amount));
}

void cam_move_backward(camera_system_t *cam, vec3 *velocity, f32 amount)
{
    vec3 bacward = mat4_backward(cam->world.view);
    *velocity = vec3_add(*velocity, vec3_scale(bacward, amount));
}

void cam_move_left(camera_system_t *cam, vec3 *velocity, f32 amount)
{
    vec3 left = mat4_left(cam->world.view);
    *velocity = vec3_add(*velocity, vec3_scale(left, amount));
}

void cam_move_right(camera_system_t *cam, vec3 *velocity, f32 amount)
{
    vec3 right = mat4_right(cam->world.view);
    *velocity = vec3_add(*velocity, vec3_scale(right, amount));
}

void cam_move_up(camera_system_t *cam, vec3 *velocity, f32 amount)
{
    (void)cam;
    vec3 up = {{0.0f, 1.0f, 0.0f}};
    *velocity = vec3_add(*velocity, vec3_scale(up, amount));
}

void cam_move_down(camera_system_t *cam, vec3 *velocity, f32 amount)
{
    (void)cam;
    vec3 down = {{0.0f, -1.0f, 0.0f}};
    *velocity = vec3_add(*velocity, vec3_scale(down, amount));
}

void cam_apply_velocity(camera_system_t *cam, vec3 velo, f32 speed, f64 delta)
{
    if (!vec3_compare(vec3_zero(), velo, 0.001f))
    {
        vec3_normalize_ptr(&velo);
        vec3 movement = vec3_scale(velo, speed * (f32)delta);
        cam->world.position = vec3_add(cam->world.position, movement);
        cam->world.dirty = true;
    }
}
