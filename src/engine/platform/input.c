#include "input.h"
#include "window.h"

// std
#include <string.h>

static input_system_t *g_ins = NULL;

static const char *key_to_string(int key, int mods);

// Input Process
static void input_process_key(input_system_t *in, int key, b8 is_press)
{
    if (key >= 0 && key < MAX_KEYS)
    {
        in->keys_curr[key] = is_press;
    }
}

static void input_process_button(input_system_t *in, int button, b8 is_press)
{
    if (button >= 0 && button < MAX_MOUSE_BUTTONS)
    {
        in->mouse_curr[button] = is_press;
    }
}

static void input_process_mouse_move(input_system_t *in, i16 pos_x, i16 pos_y)
{
    in->mouse_x = pos_x;
    in->mouse_y = pos_y;
}

static void input_process_wheel(input_system_t *in, i8 delta_z)
{
    in->mouse_wheel = delta_z;
}

// Input Callback
static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods)
{
    (void)scancode;
    input_system_t *input = glfwGetWindowUserPointer(window);
    if (!input) return;
    if (key < 0 || key > GLFW_KEY_LAST) return;

    b8 is_down = (action == GLFW_PRESS || action == GLFW_REPEAT);

    /*
    b8 old = input->keys_curr[key];
    if (!old && is_down)
        LOG_DEBUG("Key pressed: %s", key_to_string(key, mods));
    if (old && !is_down)
        LOG_DEBUG("Key released: %s", key_to_string(key, mods));
    */

    input_process_key(input, key, is_down);
    input->modifier = mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        LOG_DEBUG("ESC pressed - closing window");
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void mouse_callback(GLFWwindow *window, int button, int action,
                           int mods)
{
    (void)mods;
    input_system_t *in = glfwGetWindowUserPointer(window);
    if (!in) return;

    b8 is_down = (action == GLFW_PRESS);
    b8 old = in->mouse_curr[button];

    if (!old && is_down)
        LOG_DEBUG("Mouse button %d press at pos (%d, %d)", button, in->mouse_x,
                  in->mouse_y);
    if (old && !is_down)
        LOG_DEBUG("Mouse button %d release at pos (%d, %d)", button,
                  in->mouse_x, in->mouse_y);

    input_process_button(in, button, is_down);
}

static void wheel_callback(GLFWwindow *win, f64 xoff, f64 yoff)
{
    (void)xoff;
    input_system_t *in = glfwGetWindowUserPointer(win);
    if (!in) return;

    input_process_wheel(in, (i8)yoff);
}

static void cursor_callback(GLFWwindow *win, f64 x, f64 y)
{
    input_system_t *in = glfwGetWindowUserPointer(win);
    if (!in) return;

    input_process_mouse_move(in, (i16)x, (i16)y);

    int width, height;
    glfwGetWindowSize(win, &width, &height);
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        in->is_inside_window = false;
    }
}

static void cursor_enter_callback(GLFWwindow *window, int entered)
{
    input_system_t *in = glfwGetWindowUserPointer(window);
    if (!in) return;

    in->is_inside_window = (entered == GLFW_TRUE);
    LOG_DEBUG("Mouse %s window", entered ? "entered" : "left");
}

// Input System
input_system_t *input_sys_init(arena_alloc_t *arena)
{
    input_system_t *ins = arena_alloc(arena, sizeof(input_system_t));
    if (!ins) return NULL;
    memset(ins, 0, sizeof(input_system_t));

    ins->arena = arena;
    ins->is_inside_window = false;

    GLFWwindow *win = window_sys_get_handle();
    glfwSetWindowUserPointer(win, ins);

    glfwSetKeyCallback(win, key_callback);
    glfwSetMouseButtonCallback(win, mouse_callback);
    glfwSetCursorPosCallback(win, cursor_callback);
    glfwSetScrollCallback(win, wheel_callback);
    glfwSetCursorEnterCallback(win, cursor_enter_callback);

    g_ins = ins;
    LOG_INFO("Input System Init");
    return ins;
}

void input_sys_kill(input_system_t *ins)
{
    if (!ins) return;

    memset(ins, 0, sizeof(input_system_t));
    LOG_INFO("Input System Kill");
}

void input_sys_update(input_system_t *ins, f64 delta)
{
    (void)delta;
    memcpy(ins->keys_prev, ins->keys_curr, sizeof(ins->keys_curr));
    memcpy(ins->mouse_prev, ins->mouse_curr, sizeof(ins->mouse_curr));

    ins->mouse_wheel = 0;
}

// query keys
b8 key_held_press(int key) { return g_ins->keys_curr[key]; }

b8 key_once_pressed(int key)
{
    return (g_ins->keys_curr[key] && !g_ins->keys_prev[key]);
}

b8 key_once_released(int key)
{
    return (!g_ins->keys_curr[key] && g_ins->keys_prev[key]);
}

b8 key_mod_active(int mod) { return (g_ins->modifier & mod) != 0; }

// query mouse button
b8 mouse_held_press(int button) { return g_ins->mouse_curr[button]; }

b8 mouse_once_pressed(int button)
{
    return (g_ins->mouse_curr[button] && !g_ins->mouse_prev[button]);
}

b8 mouse_once_released(int button)
{
    return (!g_ins->mouse_curr[button] && g_ins->mouse_prev[button]);
}

void mouse_pos(int *x, int *y)
{
    *x = g_ins->mouse_x;
    *y = g_ins->mouse_y;
}

float mouse_wheel(void) { return g_ins->mouse_wheel; }

b8 mouse_inside_window(void)
{
    return g_ins ? g_ins->is_inside_window : false;
}

static const char *key_to_string(int key, int mods)
{
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
    {
        static char buf[2];
        buf[0] = (mods & GLFW_MOD_SHIFT) ? 'A' + (char)(key - GLFW_KEY_A)
                                         : 'a' + (char)(key - GLFW_KEY_A);
        buf[1] = '\0';
        return buf;
    }

    switch (key)
    {
    case GLFW_KEY_0: return (mods & GLFW_MOD_SHIFT) ? ")" : "0";
    case GLFW_KEY_1: return (mods & GLFW_MOD_SHIFT) ? "!" : "1";
    case GLFW_KEY_2: return (mods & GLFW_MOD_SHIFT) ? "@" : "2";
    case GLFW_KEY_3: return (mods & GLFW_MOD_SHIFT) ? "#" : "3";
    case GLFW_KEY_4: return (mods & GLFW_MOD_SHIFT) ? "$" : "4";
    case GLFW_KEY_5: return (mods & GLFW_MOD_SHIFT) ? "%" : "5";
    case GLFW_KEY_6: return (mods & GLFW_MOD_SHIFT) ? "^" : "6";
    case GLFW_KEY_7: return (mods & GLFW_MOD_SHIFT) ? "&" : "7";
    case GLFW_KEY_8: return (mods & GLFW_MOD_SHIFT) ? "*" : "8";
    case GLFW_KEY_9: return (mods & GLFW_MOD_SHIFT) ? "(" : "9";
    }

    switch (key)
    {
    case GLFW_KEY_LEFT_SHIFT: return "LShift";
    case GLFW_KEY_RIGHT_SHIFT: return "RShift";
    case GLFW_KEY_LEFT_CONTROL: return "LCtrl";
    case GLFW_KEY_RIGHT_CONTROL: return "RCtrl";
    case GLFW_KEY_LEFT_ALT: return "LAlt";
    case GLFW_KEY_RIGHT_ALT: return "RAlt";
    case GLFW_KEY_SPACE: return "Space";
    case GLFW_KEY_TAB: return "Tab";
    case GLFW_KEY_BACKSPACE: return "Backspace";

    case GLFW_KEY_UP: return "Up";
    case GLFW_KEY_DOWN: return "Down";
    case GLFW_KEY_RIGHT: return "Right";
    case GLFW_KEY_LEFT: return "Left";
    }

    return "Unknown";
}
