/**
 * @file game.h
 * @brief Testbed game implementation
 *
 * The actual game logic that uses the engine systems.
 * This is currently just a testbed with a spinning cube,
 * NOTE: This is temporary and will be separated from the engine later.
 */

#ifndef GAME_H
#define GAME_H

#include "engine/core/define.h"
#include "engine/rendering/camera.h"

#include "engine/system/registry.h"

// TODO: temporary
#include "engine/resource/types.h"
#include "engine/system/entity.h"

/**
 * @struct game_t
 * @brief Game state and resources
 *
 * Holds all the game-specific state. Currently very basic
 * with just a cube mesh and one entity for testing.
 */
typedef struct {
    f64 delta;
    camera_system_t *cam;
    registry_t *reg;

    // TODO: temporary
    mesh_handle_t cube;
    texture_handle_t rock;
    material_handle_t test_mat;

    entity_id entt_cube;
} game_t;

/**
 * @brief Initialize the game
 * @return Pointer to game state, or NULL if failed
 *
 * Sets up game state, creates test objects, and initializes everything.
 * Called once at startup.
 */
game_t *game_init(void);

/**
 * @brief Update game logic
 * @param game Pointer to game state
 * @param delta Time since last frame
 * @return True if game should continue running, false to quit
 *
 * Handles game logic, input, and entity updates.
 * Called once per frame before rendering.
 */
b8 game_update(game_t *game, f64 delta);

/**
 * @brief Render the game
 * @param game Pointer to game state
 * @param delta Time since last frame (for interpolation)
 * @return True if rendering succeeded, false if failed
 *
 * Renders all game objects. Called once per frame after update.
 */
b8 game_render(game_t *game, f64 delta);

/**
 * @brief Shutdown the game and clean up
 * @param game Pointer to game state
 *
 * Cleans up all game resources and shuts down systems.
 * Called once when the game is exiting.
 */
void game_kill(game_t *game);

#endif // GAME_H
