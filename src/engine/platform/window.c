#include "window.h"

// std
#include <string.h>

static window_system_t *g_ws = NULL;

static void window_clbk(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    LOG_DEBUG("Window resized to: %d x %d", width, height);
}

window_system_t *window_sys_init(arena_alloc_t *arena, int width, int height,
                                 const char *title)
{
    window_system_t *ws = arena_alloc(arena, sizeof(window_system_t));
    if (!ws) return NULL;
    memset(ws, 0, sizeof(window_system_t));

    ws->arena = arena;
    ws->width = width;
    ws->height = height;
    ws->title = title;

    if (!glfwInit()) return NULL;

    // set basic parameter
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    // hide window first, because window need to move to center of monitor
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    ws->handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!ws)
    {
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(ws->handle);
    glfwSwapInterval(0);

    // set window to center position of monitor
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (monitor)
    {
        const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
        int monitor_x, monitor_y;
        glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);

        int window_x = monitor_x + (vidmode->width - (int)ws->width) / 2;
        int window_y = monitor_y + (vidmode->height - (int)ws->height) / 2;

        glfwSetWindowPos(ws->handle, window_x, window_y);
    }

    glfwSetFramebufferSizeCallback(ws->handle, window_clbk);

    // NOTE: show window again after set window position.
    // i do like this because removing sudden movement when first
    // window appear on screen.
    glfwShowWindow(ws->handle);

    g_ws = ws;
    LOG_INFO("Window System Init");
    return ws;
}

void window_sys_kill(window_system_t *ws)
{
    glfwDestroyWindow(ws->handle);
    glfwTerminate();

    memset(ws, 0, sizeof(window_system_t));
    LOG_INFO("Window System Kill");
}

b8 window_sys_close(window_system_t *ws)
{
    return glfwWindowShouldClose(ws->handle);
}

void window_sys_poll(window_system_t *ws)
{
    (void)ws;
    glfwPollEvents();
}

void window_sys_swapbuffer(window_system_t *ws)
{
    glfwSwapBuffers(ws->handle);
}

void window_sys_get_size(int *width, int *height)
{
    glfwGetWindowSize(g_ws->handle, width, height);
}

GLFWwindow *window_sys_get_handle(void) { return g_ws->handle; }
