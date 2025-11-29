/**
 * @file app.h
 * @brief Main application structure and lifecycle management
 *
 * This is where everything comes together - the main orchestrator that ties
 * all the engine systems together. Probably has too many responsibilities
 * but hey, it works for now!
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "define.h"

#include "engine/core/clock.h"
#include "engine/core/arena.h"
#include "engine/platform/filesystem.h"
#include "engine/platform/window.h"
#include "engine/platform/input.h"
#include "engine/rendering/camera.h"
#include "engine/rendering/renderer.h"
#include "engine/module/shader.h"
#include "engine/module/lights.h"
#include "engine/module/mesh.h"
#include "engine/module/texture.h"
#include "engine/module/material.h"
#include "engine/system/registry.h"

#include "testbed/game.h"

/**
 * @struct application_t
 * @brief The main application context - holds ALL the things
 *
 * This is basically the "bag of systems" that makes the engine run.
 * If something exists in the engine, it's probably referenced here.
 * Yes, it's a bit of a god object, but it gets the job done!
 */
typedef struct {
    arena_alloc_t arena;
    clock_timer_t time;

    file_system_t *fs;
    window_system_t *ws;
    input_system_t *ip;

    camera_system_t *cs;
    render_system_t *rs;
    shader_system_t *sh;
    lighting_system_t *ls;

    mesh_system_t *ms;
    texture_system_t *ts;
    material_system_t *mts;

    // TODO: this later move to entity
    registry_t *reg;

    game_t *game;

} application_t;

/**
 * @brief Initialize the application and all its systems
 * @param app Pointer to the application context
 * @return True if initialization succeeded, false otherwise
 *
 * This is where the magic starts - sets up all the systems in the right order.
 * If this fails, probably something fundamental is broken.
 */
b8 application_init(application_t *app);

/**
 * @brief Run the main application loop
 * @param app Pointer to the application context
 * @return True if the application ran successfully, false on error
 *
 * The heart of the engine - keeps pumping frames until stop.
 * Where all the update/render magic happens in a loop.
 */
b8 application_run(application_t *app);

#endif // APPLICATION_H
