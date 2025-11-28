#include "filesystem.h"

// std
#include <stdio.h>
#include <sys/stat.h>

#if PLATFORM_LINUX
#    include <unistd.h>
#elif PLATFORM_WINDOWS
#    include <direct.h>
#endif

static file_system_t *g_fs = NULL;

static path_t get_executable_dir(void)
{
    char buffer[MAX_PATH];

#if PLATFORM_LINUX
    ssize_t len = readlink("/proc/self/exe", buffer, MAX_PATH - 1);
    if (len < 0 || len >= MAX_PATH) LOG_FATAL("failed to read /proc/self/exe");

    buffer[len] = '\0';

#elif PLATFORM_WINDOWS
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0 || len == MAX_PATH) LOG_FATAL("GetModuleFileNameA failed");
#endif

    // strip filename
#ifdef PLATFORM_LINUX
    char *slash = strrchr(buffer, '/');
#elif PLATFORM_WINDOWS
    char *slash = strrchr(buffer, '\\'); // but normalize() handles it anyway
#endif

    if (slash) *slash = '\0';

    return path_normalize(buffer);
}

file_system_t *file_system_init(arena_alloc_t *arena)
{
    file_system_t *fs = arena_alloc(arena, sizeof(file_system_t));
    if (!fs) return NULL;
    memset(fs, 0, sizeof(file_system_t));

    fs->arena = arena;

    path_t exe_dir = get_executable_dir();
    LOG_TRACE("exe_dir = %s", exe_dir.buffer);

    // Assets folder next to executable
    path_t p1 = path_join(exe_dir.buffer, "assets");
    if (file_exist(p1.buffer))
    {
        fs->base_path = p1;
        fs->is_available = true;
        g_fs = fs;
        LOG_INFO("File System Init base-path = %s", fs->base_path.buffer);
        return fs;
    }

    // Dev fallback: ../assets (useful when running from bin/)
    path_t p2 = path_join(exe_dir.buffer, "../assets");
    if (file_exist(p2.buffer))
    {
        fs->base_path = p2;
        fs->is_available = true;
        g_fs = fs;
        LOG_INFO("File System Init base-path (fallback) = %s",
                 fs->base_path.buffer);
        return fs;
    }

    // Failure
    LOG_ERROR("could not locate assets directory.");
    LOG_ERROR("  checked:");
    LOG_ERROR("    %s", p1.buffer);
    LOG_ERROR("    %s", p2.buffer);

    fs->is_available = true;
    g_fs = fs;
    return NULL;
}

void file_system_kill(file_system_t *fs)
{
    if (!fs) return;
    memset(fs, 0, sizeof(file_system_t));
    LOG_INFO("File System Kill");
}

b8 file_exist(const char *path)
{
    struct stat buffer;
    return stat(path, &buffer) == 0;
}

b8 file_open(const char *path, filemode_t mode, file_t *handle)
{
    handle->is_valid = false;
    handle->handle = NULL;
    const char *mode_str = NULL;

    b8 is_read = (mode & (READ_TEXT | READ_BINARY)) != 0;
    b8 is_write = (mode & (WRITE_TEXT | WRITE_BINARY)) != 0;
    b8 is_binary = (mode & (READ_BINARY | WRITE_BINARY)) != 0;

    if (is_read && is_write)
    {
        LOG_ERROR("cannot open file '%s' in both read & write mode", path);
        return false;
    }

    if (is_read)
        mode_str = is_binary ? "rb" : "r";
    else if (is_write)
        mode_str = is_binary ? "wb" : "w";

    if (!mode_str)
    {
        LOG_ERROR("invalid file mode flags for '%s'", path);
        return false;
    }

    char full_path[MAX_PATH];
    path_t joined = path_join(g_fs->base_path.buffer, path);
    strncpy(full_path, joined.buffer, MAX_PATH);
    FILE *file = fopen(full_path, mode_str);
    if (!file)
    {
        LOG_ERROR("error open file '%s'", full_path);
        return false;
    }

    handle->handle = file;
    handle->is_valid = true;
    return true;
}

void file_close(file_t *handle)
{
    if (handle->is_valid && handle->handle)
    {
        fclose((FILE *)handle->handle);
        handle->handle = NULL;
        handle->is_valid = false;
    }
}

b8 file_size(file_t *handle, u64 *size)
{
    if (handle->handle)
    {
        fseek((FILE *)handle->handle, 0, SEEK_END);
        *size = (u64)ftell((FILE *)handle->handle);
        rewind((FILE *)handle->handle);
        return true;
    }
    return false;
}

b8 file_read_all_text(file_t *handle, char *text, u64 *out_read)
{
    if (handle->handle && text && out_read)
    {
        u64 size = 0;
        if (!file_size(handle, &size)) return false;

        *out_read = fread(text, 1, size, (FILE *)handle->handle);
        return *out_read == size;
    }
    return false;
}

b8 file_read_all_binary(file_t *handle, u8 *out_byte, u64 *out_read)
{
    if (handle->handle && out_byte && out_read)
    {
        u64 size = 0;
        if (!file_size(handle, &size)) return false;

        *out_read = fread(out_byte, 1, size, (FILE *)handle->handle);
        return *out_read == size;
    }
    return false;
}
