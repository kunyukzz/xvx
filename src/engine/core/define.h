#ifndef DEFINE_H
#define DEFINE_H

#ifndef PLATFORM_LINUX
#    if defined(__linux__) || defined(__gnu_linux__)
#        define PLATFORM_LINUX 1
#        ifndef _POSIX_C_SOURCE
#            define _POSIX_C_SOURCE 200809L
#        endif
#    endif
#endif

#ifndef PLATFORM_WINDOWS
#    if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#        define PLATFORM_WINDOWS 1
#    endif
#endif

#if defined(__clang__) || defined(__gcc__)
#    define STATIC_ASSERT(cond, msg)                                          \
        typedef char static_assertion_##msg[(cond) ? 1 : -1]
#else
#    define STATIC_ASSERT static_assert
#endif

#define CLAMP(v, min, max) (v <= min) ? min : (v >= max) ? max : v
#define LERP(a, b, t) ((a) + ((b) - (a)) * (t))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define INVALID_8 0xFFu
#define INVALID_16 0xFFFu
#define INVALID_32 0xFFFFFFFFu
#define INVALID_64 0xFFFFFFFFFFFFFFFFu

#define true 1
#define false 0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
typedef float f32;
typedef double f64;
typedef _Bool b8;
typedef unsigned long uptr;

STATIC_ASSERT(sizeof(u8) == 1, u8_must_be_1_byte);
STATIC_ASSERT(sizeof(u16) == 2, u16_must_be_2_byte);
STATIC_ASSERT(sizeof(u32) == 4, u32_must_be_4_byte);
STATIC_ASSERT(sizeof(u64) == 8, u64_must_be_8_byte);
STATIC_ASSERT(sizeof(i8) == 1, i8_must_be_1_byte);
STATIC_ASSERT(sizeof(i16) == 2, i16_must_be_2_byte);
STATIC_ASSERT(sizeof(i32) == 4, i32_must_be_4_byte);
STATIC_ASSERT(sizeof(i64) == 8, i64_must_be_8_byte);
STATIC_ASSERT(sizeof(uptr) == 8, uptr_must_be_8_byte);

#if defined(__clang__) || defined(__gcc__)
#    define INL __attribute__((always_inline)) inline
#    define NOINL __attribute__((noinline))
#    define ALIGN(n) __attribute__((aligned(n)))
#    define LIKELY(x) __builtin_expect(!!(x), 1)
#    define UNLIKELY(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER)
#    define INL __forceinline
#    define NOINL __declspec(noinline)
#    define ALIGN(n) __declspec(align(n))
#    define LIKELY(x) (x)
#    define UNLIKELY(x) (x)
#endif

#ifdef DEBUG
#    include <assert.h>
#    define ASSERT(condition, message) assert((condition) && message)
#    define CHECK_VK(result) assert((result) == VK_SUCCESS)
#else
#    define ASSERT(condition, message) ((void)0)
#    define CHECK_VK(result) ((void)0)
#endif

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

void log_msg(log_level_t level, const char *msg, ...);

#define LOG_TRACE(msg, ...) log_msg(LOG_TRACE, msg, ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) log_msg(LOG_DEBUG, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) log_msg(LOG_INFO, msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...) log_msg(LOG_WARN, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) log_msg(LOG_ERROR, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) log_msg(LOG_FATAL, msg, ##__VA_ARGS__)

#define UNUSED(x) ((void)(x))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define OFFSETOF(type, member) ((size_t)&((type *)0)->member)

#endif // DEFINE_H
