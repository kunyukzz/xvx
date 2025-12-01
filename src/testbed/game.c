#include "game.h"
#include "engine/core/memory.h"
#include "engine/platform/input.h"
#include "engine/math/maths.h"

#include "engine/module/mesh.h"
#include "engine/module/texture.h"
#include "engine/module/material.h"

game_t *game_init(void)
{
    game_t *game = ALLOC(sizeof(game_t), MEM_GAME);

    game->cam = get_camera_system();
    game->reg = get_registry_system();

    // game->cube = mesh_create_cube(1.0, 1.0, 1.0);
    // game->rock = texture_create_from_file("textures/rocks");
    /*
    game->test_mat = material_create("rock", (vec4){{0.8f, 0.0f, 0.0f, 1.0f}},
                                     0, 0.0f, 0.0f);
    */

    mesh_handle_t qubu = mesh_create_cube(1.0, 1.0, 1.0);
    texture_handle_t rock = texture_create_from_file("textures/rocks");
    material_handle_t stone_rock = material_create(
        "test", (vec4){{0.8f, 0.0f, 0.0f, 1.0f}}, rock, 0.0f, 0.0f);

    mesh_handle_t floor_handle = mesh_create_plane(2.0, 2.0, 1);
    material_handle_t floor_mat = material_create(
        "floor", (vec4){{1.0f, 1.0f, 1.0f, 1.0f}}, 0, 0.0, 0.0);

    game->entt_cube = registry_create_entity(game->reg);
    game->entt_floor = registry_create_entity(game->reg);

    transform_comp_t t1 = transform_comp_default();
    transform_comp_t t2 = transform_comp_default();
    registry_add_transform(game->reg, game->entt_cube, t1);
    registry_add_transform(game->reg, game->entt_floor, t2);

    model_comp_t rock_model = model_comp_create(qubu, stone_rock);
    registry_add_model(game->reg, game->entt_cube, rock_model);

    model_comp_t floor_model = model_comp_create(floor_handle, floor_mat);
    registry_add_model(game->reg, game->entt_floor, floor_model);

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
