/**
 * @file paths.h
 * @brief Simple path manipulation utilities for cross-platform file handling
 *
 * Basic path normalization, joining, and absolute/relative path checking.
 * Designed for games and applications that use a simple "assets next to exe"
 * approach.
 *
 * @note Maximum path length: MAX_PATH
 * @note Converts all backslashes to forward slashes internally
 * @note Handles "." and ".." components during normalization
 * @note Always returns absolute paths starting with '/'
 */

#ifndef PATH_H
#define PATH_H

#include "define.h"
#include <string.h>

#define MAX_PATH 512

typedef struct {
    char buffer[MAX_PATH];
} path_t;

INL path_t path_normalize(const char *raw_path)
{
    path_t out;
    char temp[MAX_PATH];
    u32 len = 0;

    // convert backslashes to forward slashes and copy to temp buffer
    for (u32 i = 0; raw_path[i] && i < MAX_PATH - 1; ++i)
    {
        temp[i] = (raw_path[i] == '\\') ? '/' : raw_path[i];
        len = i + 1;
    }
    temp[len] = '\0';

    // tokenize the path (split on '/')
    char *tokens[128];
    u32 token_count = 0;

    const char *token = temp;
    while (*token && token_count < 128)
    {
        // skip consecutive slashes
        while (*token == '/') ++token;
        if (*token == '\0') break;

        // find next token
        char *token_start = (char *)token;
        while (*token && *token != '/') ++token;

        // null-terminate token and move past it
        if (*token)
        {
            *((char *)token) = '\0';
            ++token;
        }

        // handle special tokens: ".", "..", and normal paths
        if (strcmp(token_start, ".") == 0)
        {
            // current directory, skip it.
        }
        else if (strcmp(token_start, "..") == 0)
        {
            // parent directory - go up one level if possible
            if (token_count > 0) token_count--;
        }
        else
        {
            // regular path - add to tokens
            tokens[token_count++] = token_start;
        }
    }

    // rebuild normalized path from tokens
    len = 0;
    for (u32 i = 0; i < token_count; ++i)
    {
        u32 token_len = (u32)strlen(tokens[i]);

        if (len + token_len + 1 < MAX_PATH)
        {
            out.buffer[len++] = '/';
            memcpy(&out.buffer[len], tokens[i], token_len);
            len += token_len;
        }
    }

    // handle empty path case (return root)
    if (len == 0)
    {
        out.buffer[0] = '/';
        len = 1;
    }

    out.buffer[len] = '\0';
    return out;
}

INL path_t path_join(const char *base, const char *extra)
{
    // bail if extra path is NULL or empty
    if (extra == NULL || extra[0] == '\0')
    {
        LOG_FATAL("called with empty subpath");
        path_t empty = {0};
        return empty;
    }

    char combined[MAX_PATH];
    u32 len = 0;

    // copy base path
    for (; *base && len < MAX_PATH - 1; ++base) combined[len++] = *base;

    // add separator if base doesn't end with one
    if (len > 0 && combined[len - 1] != '/') combined[len++] = '/';

    // append those extra path
    for (; *extra && len < MAX_PATH - 1; ++extra) combined[len++] = *extra;

    combined[len] = '\0';
    return path_normalize(combined);
}

INL b8 path_is_absolute(const char *path)
{
#ifdef PLATFORM_LINUX
    return path[0] == '/';
#elif PLATFORM_WINDOWS
    return (path[0] && path[1] == ':') || (path[0] == '\\' || path[0] == '/');
#endif
}

INL b8 path_is_relative(const char *path) { return !path_is_absolute(path); }

#endif // PATH_H
