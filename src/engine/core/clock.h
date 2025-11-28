/**
 * @file clock.h
 * @brief Simple timing utilities for frame management and profiling
 *
 * This is my attempt at cross-platform timing. It mostly works, except when it
 * doesn't. Uses GLFW for time because it's easier than dealing with
 * platform-specific nonsense.
 */

#ifndef CLOCK_TIMER_H
#define CLOCK_TIMER_H

#include "define.h"

#if PLATFORM_LINUX
#    include <time.h>
#elif PLATFORM_WINDOWS
#    include <Windows.h>
#endif

#include <GLFW/glfw3.h>

#include <string.h>

/**
 * @struct clock_timer_t
 * @brief Simple timer for measuring elapsed time
 *
 * This is about as basic as it gets - start time and elapsed time.
 * No fancy features, just enough to know how long things take.
 */
typedef struct {
    f64 start_time;
    f64 elapsed;
} clock_timer_t;

/**
 * @brief Start the timer
 * @param timer Pointer to timer to start
 *
 * Resets the timer and starts counting from now.
 */
INL void timer_start(clock_timer_t *timer)
{
    timer->start_time = glfwGetTime();
    timer->elapsed = 0.0;
}

/**
 * @brief Update the timer with current elapsed time
 * @param timer Pointer to timer to update
 *
 * Call this every frame to keep the elapsed time current.
 */
INL void timer_update(clock_timer_t *timer)
{
    timer->elapsed = glfwGetTime() - timer->start_time;
}

/**
 * @brief Stop and reset the timer
 * @param timer Pointer to timer to stop
 *
 * Resets everything to zero.
 * Useful when you don't want to know how long something took anymore.
 */
INL void timer_stop(clock_timer_t *timer)
{
    timer->start_time = 0.0;
    timer->elapsed = 0.0;
}

/**
 * @brief Get the current global time
 * @return Current time in seconds since... something started
 *
 * Just wraps glfwGetTime() because I'm lazy.
 * Good for when you need absolute time rather than relative.
 */
INL f64 timer_get() { return glfwGetTime(); }

/**
 * @brief Sleep until a specific time
 * @param wake Time to wake up (in seconds)
 *
 * My attempt at frame limiting. Uses fancy nanosleep on Linux
 * and the boring Sleep() on Windows. Probably not super accurate
 * but good enough for games (I hope).
 */
INL void get_sleep(f64 wake)
{
#if PLATFORM_LINUX
    struct timespec ts;
    ts.tv_sec = (time_t)wake;
    ts.tv_nsec = (long)((wake - (f64)ts.tv_sec) * 1e9);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
#elif PLATFORM_WINDOWS
    f64 now = glfwGetTime();
    f64 sleep_sec = wake - now;
    if (sleep_sec <= 0.0) return;
    Sleep((DWORD)(sleep_sec * 1000.0));
#endif
}

#endif // CLOCK_TIMER_H
