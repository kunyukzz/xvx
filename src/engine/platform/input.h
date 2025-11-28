/**
 * @file input.h
 * @brief Input system for keyboard and mouse handling
 *
 * Basic input handling using GLFW. Tracks key states with current/previous
 * frames so we can detect presses and releases. Also handles mouse position
 * and wheel.
 */

#ifndef INPUT_H
#define INPUT_H

#include "engine/core/define.h"
#include "engine/core/arena.h"
#include <GLFW/glfw3.h>

#define MAX_KEYS (GLFW_KEY_LAST + 1)
#define MAX_MOUSE_BUTTONS GLFW_MOUSE_BUTTON_LAST

/**
 * @struct input_system_t
 * @brief Input system state tracking
 *
 * Tracks keyboard and mouse state across frames to detect changes.
 * Uses double-buffering (current/previous) to determine presses and releases.
 * The arena is mostly for future expansion... or just for consistency!
 */
typedef struct {
    arena_alloc_t *arena;
    b8 keys_curr[MAX_KEYS];
    b8 keys_prev[MAX_KEYS];
    int modifier;

    b8 mouse_curr[MAX_MOUSE_BUTTONS];
    b8 mouse_prev[MAX_MOUSE_BUTTONS];
    int mouse_x, mouse_y;
    f32 mouse_wheel;

    b8 is_inside_window;
} input_system_t;

/**
 * @brief Initialize the input system
 * @param arena Memory arena for allocations
 * @return Pointer to input system, or NULL if failed
 *
 * Sets up input system and registers GLFW callbacks.
 * Also initializes all key states to "not pressed".
 */
input_system_t *input_sys_init(arena_alloc_t *arena);

/**
 * @brief Shutdown the input system
 * @param ins Pointer to input system
 *
 * Cleans up input system. Mostly just resets everything to zero.
 */
void input_sys_kill(input_system_t *ins);

/**
 * @brief Update input system state
 * @param ins Pointer to input system
 * @param delta Time since last frame (unused for now)
 *
 * Copies current frame states to previous frame, and updates mouse wheel.
 */
void input_sys_update(input_system_t *ins, f64 delta);

/**
 * @brief Check if a key is currently held down
 * @param key GLFW key code (GLFW_KEY_A, GLFW_KEY_SPACE, etc.)
 * @return True if key is pressed this frame
 *
 * Use this for continuous input like character movement.
 */
b8 key_held_press(int key);

/**
 * @brief Check if a key was pressed this frame
 * @param key GLFW key code
 * @return True if key was pressed this frame (was up last frame, down this
 * frame)
 *
 * Use this for one-time actions like jumping or shooting.
 */
b8 key_once_pressed(int key);

/**
 * @brief Check if a key was released this frame
 * @param key GLFW key code
 * @return True if key was released this frame (was down last frame, up this
 * frame)
 *
 * Use this for detecting when the user stops pressing a key.
 */
b8 key_once_released(int key);

/**
 * @brief Check if modifier keys are active
 * @param mod Modifier mask (GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, etc.)
 * @return True if the specified modifier is currently held
 *
 * Useful for detecting shift, ctrl, alt key combinations.
 */
b8 key_mod_active(int mod);

/**
 * @brief Check if mouse button is currently held
 * @param button GLFW mouse button (GLFW_MOUSE_BUTTON_LEFT, etc.)
 * @return True if mouse button is pressed this frame
 *
 * Use this for continuous mouse actions like dragging.
 */
b8 mouse_held_press(int button);

/**
 * @brief Check if mouse button was pressed this frame
 * @param button GLFW mouse button
 * @return True if mouse button was pressed this frame
 *
 * Use this for mouse clicks that should happen once per press.
 */
b8 mouse_once_pressed(int button);

/**
 * @brief Check if mouse button was released this frame
 * @param button GLFW mouse button
 * @return True if mouse button was released this frame
 *
 * Use this for detecting when mouse button is released.
 */
b8 mouse_once_released(int button);

/**
 * @brief Get current mouse position
 * @param x Output for mouse X coordinate
 * @param y Output for mouse Y coordinate
 *
 * Gets the current mouse position relative to the window.
 */
void mouse_pos(int *x, int *y);

/**
 * @brief Get mouse wheel delta
 * @return Mouse wheel movement since last frame
 *
 * Positive values = scroll up, negative values = scroll down.
 * Resets to zero each frame, so you need to use it when it happens!
 */
f32 mouse_wheel(void);

/**
 * @brief Check if mouse is inside the window
 * @return True if mouse cursor is inside our window
 *
 * Useful for ignoring mouse input when the cursor is outside the window.
 */
b8 mouse_inside_window(void);

#endif // INPUT_H
