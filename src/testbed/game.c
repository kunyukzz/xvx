#include "game.h"
#include "engine/core/memory.h"
#include "engine/platform/input.h"
#include "engine/math/maths.h"

#include "engine/module/mesh.h"

game_t *game_init(void)
{
    game_t *game = ALLOC(sizeof(game_t), MEM_GAME);

    game->cam = get_camera_system();
    game->cube = mesh_create_cube(3.0, 3.0, 3.0);

    LOG_INFO("Game Init");
    return game;
}

b8 game_update(game_t *game, f64 delta)
{
    f32 rotate_speed = 5.0f * (f32)delta;
    f32 move_speed = 5.0f;
    f32 amount = 1.0f;
    vec3 velo = vec3_zero();

    if (key_held_press(GLFW_KEY_LEFT)) cam_yaw(game->cam, rotate_speed);
    if (key_held_press(GLFW_KEY_RIGHT)) cam_yaw(game->cam, -rotate_speed);
    if (key_held_press(GLFW_KEY_UP)) cam_pitch(game->cam, rotate_speed);
    if (key_held_press(GLFW_KEY_DOWN)) cam_pitch(game->cam, -rotate_speed);

    if (key_held_press(GLFW_KEY_W))
    {
        cam_move_forward(game->cam, &velo, amount);
    }
    if (key_held_press(GLFW_KEY_S))
    {
        cam_move_backward(game->cam, &velo, amount);
    }
    if (key_held_press(GLFW_KEY_A))
    {
        cam_move_left(game->cam, &velo, amount);
    }
    if (key_held_press(GLFW_KEY_D))
    {
        cam_move_right(game->cam, &velo, amount);
    }
    if (key_held_press(GLFW_KEY_Q))
    {
        cam_move_up(game->cam, &velo, amount);
    }
    if (key_held_press(GLFW_KEY_E))
    {
        cam_move_down(game->cam, &velo, amount);
    }

    cam_apply_velocity(game->cam, velo, move_speed, delta);

    return true;
}

b8 game_render(game_t *game, f64 delta)
{
    (void)game;
    (void)delta;
    return true;
}

void game_kill(game_t *game)
{
    if (!game) return;
    FREE(game, sizeof(game_t), MEM_GAME);
    LOG_INFO("Game Kill");
}
