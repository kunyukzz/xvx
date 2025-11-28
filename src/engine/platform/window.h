/**
 * @file window.h
 * @brief Window system for creating and managing GLFW windows
 *
 * Basic window management using GLFW.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "engine/core/define.h"
#include "engine/core/arena.h"

#include <GLFW/glfw3.h>

/**
 * @struct window_system_t
 * @brief Window system state and context
 *
 * Wraps GLFW window with some basic metadata.
 * The arena is probably overkill but I like consistency!
 */
typedef struct {
    arena_alloc_t *arena;
    GLFWwindow *handle;
    int width, height;
    const char *title;
} window_system_t;

/**
 * @brief Initialize the window system
 * @param arena Memory arena for allocations
 * @param width Initial window width
 * @param height Initial window height
 * @param title Window title
 * @return Pointer to window system, or NULL if GLFW failed
 *
 * Creates a GLFW window with OpenGL context. This only sets up callbacks
 * resize events.
 */
window_system_t *window_sys_init(arena_alloc_t *arena, int width, int height,
                                 const char *title);

/**
 * @brief Shutdown the window system
 * @param ws Pointer to window system
 *
 * Destroys the GLFW window and terminates GLFW.
 */
void window_sys_kill(window_system_t *ws);

/**
 * @brief Check if window should close
 * @param ws Pointer to window system
 * @return True if window close requested (X button clicked)
 *
 * Checks if the user wants to close the window.
 * The main loop should stop when this returns true.
 */
b8 window_sys_close(window_system_t *ws);

/**
 * @brief Poll for window events
 * @param ws Pointer to window system
 *
 * Processes pending window events (input, resize, etc.).
 */
void window_sys_poll(window_system_t *ws);

/**
 * @brief Swap front and back buffers
 * @param ws Pointer to window system
 *
 * Presents the rendered frame to the screen.
 */
void window_sys_swapbuffer(window_system_t *ws);

/**
 * @brief Get current window size
 * @param width Output for current width
 * @param height Output for current height
 *
 * Useful for handling resize events or aspect ratio calculations.
 */
void window_sys_get_size(int *width, int *height);

/**
 * @brief Get the raw GLFW window handle
 * @return Pointer to GLFW window, or NULL if not initialized
 *
 * For when you need to do GLFW-specific things that I didn't wrap.
 */
GLFWwindow *window_sys_get_handle(void);

#endif // WINDOW_H
