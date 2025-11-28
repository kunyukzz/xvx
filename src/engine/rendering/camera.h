/**
 * @file camera.h
 * @brief Camera system for view and projection matrices
 *
 * Handles camera movement, rotation, and matrix calculations.
 * Currently has first-person style controls for development,
 * but will be removed later for fixed-position tycoon cameras.
 * The camera gets marked as "dirty" when it moves so we only recalc matrices
 * when needed.
 */

#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include "engine/math/math_types.h"

/**
 * @enum camera_projection_type_t
 * @brief Camera projection types
 *
 * PERSPECTIVE for 3D view with depth, ORTHOGRAPHIC for 2D/sim view.
 */
typedef enum {
    CAMERA_PROJECTION_PERSPECTIVE,
    CAMERA_PROJECTION_ORTHOGRAPHIC
} camera_projection_type_t;

/**
 * @struct camera_t
 * @brief Camera state and matrices
 *
 * Stores camera position, orientation, and pre-calculated matrices.
 * Has a dirty flag to avoid recalculating matrices every frame.
 */
typedef struct {
    mat4 proj;
    mat4 view;

    f32 near, far, fov;
    f32 aspect_ratio;
    vec3 position, rotation;
    vec3 front, up, right;

    camera_projection_type_t proj_type;

    b8 dirty;
} camera_t;

/**
 * @struct camera_system_t
 * @brief Camera system context
 *
 * Currently just holds the world camera. Might expand later for multiple
 * cameras. The arena is for future dynamic camera allocation.
 */
typedef struct {
    arena_alloc_t *arena;
    camera_t world;
} camera_system_t;

/**
 * @brief Initialize the camera system
 * @param arena Memory arena for allocations
 * @return Pointer to camera system, or NULL if failed
 *
 * Sets up the default camera with perspective projection.
 * Positioned at origin looking down negative Z.
 */
camera_system_t *camera_sys_init(arena_alloc_t *arena);

/**
 * @brief Shutdown the camera system
 * @param cs Pointer to camera system
 *
 * Cleans up the camera system. Mostly just resets everything.
 */
void camera_sys_kill(camera_system_t *cs);

/**
 * @brief Update camera matrices if needed
 * @param cs Pointer to camera system
 *
 * Recalculates view and projection matrices if the camera is dirty.
 * Call this before rendering each frame.
 */
void camera_update(camera_system_t *cs);

/**
 * @brief Get the global camera system
 * @return Pointer to camera system
 *
 * Global accessor for the camera system.
 * Might be a bad idea but it's convenient for now!
 */
camera_system_t *get_camera_system(void);

/**
 * @brief Rotate camera horizontally (yaw)
 * @param cam Pointer to camera system
 * @param amount Rotation amount in radians
 *
 * Rotates the camera left/right. Temporary for development.
 * Will be removed when we switch to fixed tycoon camera.
 */
void cam_yaw(camera_system_t *cam, f32 amount);

/**
 * @brief Rotate camera vertically (pitch)
 * @param cam Pointer to camera system
 * @param amount Rotation amount in radians
 *
 * Rotates the camera up/down. Temporary for development.
 * Will be removed when we switch to fixed tycoon camera.
 */
void cam_pitch(camera_system_t *cam, f32 amount);

/**
 * @brief Move camera forward
 * @param cam Pointer to camera system
 * @param velocity Velocity vector to modify
 * @param amount Movement amount
 *
 * Adds forward movement to velocity. Temporary for development.
 */
void cam_move_forward(camera_system_t *cam, vec3 *velocity, f32 amount);

/**
 * @brief Move camera backward
 * @param cam Pointer to camera system
 * @param velocity Velocity vector to modify
 * @param amount Movement amount
 *
 * Adds backward movement to velocity. Temporary for development.
 */
void cam_move_backward(camera_system_t *cam, vec3 *velocity, f32 amount);

/**
 * @brief Move camera left
 * @param cam Pointer to camera system
 * @param velocity Velocity vector to modify
 * @param amount Movement amount
 *
 * Adds left movement to velocity. Temporary for development.
 */
void cam_move_left(camera_system_t *cam, vec3 *velocity, f32 amount);

/**
 * @brief Move camera right
 * @param cam Pointer to camera system
 * @param velocity Velocity vector to modify
 * @param amount Movement amount
 *
 * Adds right movement to velocity. Temporary for development.
 */
void cam_move_right(camera_system_t *cam, vec3 *velocity, f32 amount);

/**
 * @brief Move camera up
 * @param cam Pointer to camera system
 * @param velocity Velocity vector to modify
 * @param amount Movement amount
 *
 * Adds upward movement to velocity. Temporary for development.
 */
void cam_move_up(camera_system_t *cam, vec3 *velocity, f32 amount);

/**
 * @brief Move camera down
 * @param cam Pointer to camera system
 * @param velocity Velocity vector to modify
 * @param amount Movement amount
 *
 * Adds downward movement to velocity. Temporary for development.
 */
void cam_move_down(camera_system_t *cam, vec3 *velocity, f32 amount);

/**
 * @brief Apply velocity to camera position
 * @param cam Pointer to camera system
 * @param velo Velocity vector
 * @param speed Movement speed multiplier
 * @param delta Time since last frame
 *
 * Applies accumulated velocity to camera position and marks camera as dirty.
 * Temporary for development - will be removed later.
 */
void cam_apply_velocity(camera_system_t *cam, vec3 velo, f32 speed, f64 delta);

#endif // CAMERA_SYSTEM_H
