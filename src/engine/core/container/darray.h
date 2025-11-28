/**
 * @file darray.h
 * @brief Dynamic array implementation for flexible data storage
 *
 * A simple dynamic array that grows automatically when needed.
 * Useful for when you don't know how many elements you'll need upfront.
 * Much simpler than C++ vectors but gets the job done!
 */

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "engine/core/define.h"

/**
 * @struct darray_t
 * @brief Dynamic array structure
 *
 * Stores elements in a contiguous block that grows as needed.
 * Keeps track of size, capacity, and element size for proper memory
 * management.
 */
typedef struct {
    void *data;
    u64 size;
    u64 capacity;
    u64 element_size;
} darray_t;

/**
 * @brief Create a new dynamic array
 * @param init_capacity Initial capacity (number of elements)
 * @param element_size Size of each element in bytes
 * @return Pointer to new dynamic array, or NULL if allocation failed
 *
 * Allocates a new dynamic array with the specified initial capacity.
 * The array will grow automatically when it runs out of space.
 */
darray_t *da_create(u64 init_capacity, u64 element_size);

/**
 * @brief Append an element to the array
 * @param da Pointer to dynamic array
 * @param element Pointer to element data to append
 * @return Index where the element was inserted, or (u64)-1 if failed
 *
 * Adds an element to the end of the array. Grows the array if necessary.
 * Returns the index of the new element, which can be used with da_get().
 */
u64 da_append(darray_t *da, const void *element);

/**
 * @brief Get an element from the array
 * @param da Pointer to dynamic array
 * @param index Index of the element to retrieve
 * @return Pointer to the element, or NULL if index is out of bounds
 *
 * Returns a pointer to the element at the specified index.
 * The pointer remains valid until the array is modified or freed.
 */
void *da_get(const darray_t *da, u64 index);

/**
 * @brief Set an element in the array
 * @param da Pointer to dynamic array
 * @param index Index of the element to set
 * @param element Pointer to new element data
 * @return The index if successful, or (u64)-1 if index is out of bounds
 *
 * Copies new data into an existing element in the array.
 * Does not grow the array - index must be within current size.
 */
u64 da_set(darray_t *da, u64 index, const void *element);

/**
 * @brief Free the dynamic array and its data
 * @param da Pointer to dynamic array to free
 *
 * Frees all memory associated with the dynamic array.
 * The pointer becomes invalid after this call.
 */
void da_free(darray_t *da);

/**
 * @brief Get the current size of the array
 * @param da Pointer to dynamic array
 * @return Number of elements currently in the array
 */
u64 da_size(const darray_t *da);

/**
 * @brief Get the current capacity of the array
 * @param da Pointer to dynamic array
 * @return Total capacity of the array (number of elements it can hold)
 */
u64 da_capacity(const darray_t *da);

#endif // DYNAMIC_ARRAY_H
