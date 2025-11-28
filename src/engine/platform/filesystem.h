/**
 * @file filesystem.h
 * @brief Basic file system operations for reading game assets
 *
 * Simple file I/O wrapper because dealing with platform-specific file APIs is
 * annoying. Can read text and binary files, which covers most game asset
 * needs. Probably not the most efficient, but it works!
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "engine/core/define.h"
#include "engine/core/paths.h"
#include "engine/core/arena.h"

/**
 * @struct file_t
 * @brief Opaque file handle for file operations
 *
 * Platform-specific file handle hidden behind a simple struct.
 * The is_valid flag saves you from using invalid handles.
 */
typedef struct {
    void *handle;
    b8 is_valid;
} file_t;

/**
 * @enum filemode_t
 * @brief File opening modes as bit flags
 *
 * Different ways to open files. Can be combined for flexibility.
 * Text mode handles newline conversions, binary mode doesn't.
 */
typedef enum {
    READ_TEXT = 0x01,
    READ_BINARY = 0x02,
    WRITE_TEXT = 0x04,
    WRITE_BINARY = 0x08
} filemode_t;

/**
 * @struct file_system_t
 * @brief File system state and context
 *
 * Tracks the base path for asset loading and availability.
 * The arena is used for path strings and other allocations.
 */
typedef struct {
    arena_alloc_t *arena;
    path_t base_path;
    b8 is_available;
} file_system_t;

/**
 * @brief Initialize the file system
 * @param arena Memory arena for allocations
 * @return Pointer to file system, or NULL if failed
 *
 * Sets up the file system and determines the base path for assets.
 * If this fails, you probably can't load any game files!
 */
file_system_t *file_system_init(arena_alloc_t *arena);

/**
 * @brief Shutdown the file system
 * @param fs Pointer to file system
 *
 * Cleans up file system resources. Any open files should be closed first!
 */
void file_system_kill(file_system_t *fs);

// utilities
/**
 * @brief Check if a file exists
 * @param path Path to the file
 * @return True if file exists and is accessible
 *
 * Useful for checking if assets are present before trying to load them.
 */
b8 file_exist(const char *path);

/**
 * @brief Open a file
 * @param path Path to the file
 * @param mode File opening mode (read/write, text/binary)
 * @param handle Output file handle
 * @return True if file opened successfully
 *
 * Opens a file with the specified mode. Must be closed with file_close()!
 */
b8 file_open(const char *path, filemode_t mode, file_t *handle);

/**
 * @brief Close a file
 * @param handle File handle to close
 *
 * Closes an open file. Always call this when done with a file!
 */
void file_close(file_t *handle);

/**
 * @brief Get file size
 * @param handle Open file handle
 * @param size Output for file size in bytes
 * @return True if size was retrieved successfully
 *
 * Useful for allocating the right amount of memory before reading a file.
 */
b8 file_size(file_t *handle, u64 *size);

/**
 * @brief Read entire file as text
 * @param handle Open file handle (must be opened with READ_TEXT)
 * @param text Output buffer for text data
 * @param out_read Output for number of bytes actually read
 * @return True if read succeeded
 *
 * Reads the entire file into a text buffer. The buffer must be large enough!
 * Text mode handles platform-specific newline conversions.
 */
b8 file_read_all_text(file_t *handle, char *text, u64 *out_read);

/**
 * @brief Read entire file as binary
 * @param handle Open file handle (must be opened with READ_BINARY)
 * @param out_byte Output buffer for binary data
 * @param out_read Output for number of bytes actually read
 * @return True if read succeeded
 *
 * Reads the entire file into a binary buffer. Useful for images, models, etc.
 * The buffer must be pre-allocated with enough space!
 */
b8 file_read_all_binary(file_t *handle, u8 *out_byte, u64 *out_read);

#endif // FILESYSTEM_H
