#include "darray.h"
#include "engine/core/memory.h"

// std
#include <string.h>

darray_t *da_create(u64 init_capacity, u64 element_size)
{
    darray_t *da = ALLOC(sizeof(darray_t), MEM_DYNARRAY);
    if (!da) return NULL;

    da->data = ALLOC(init_capacity * element_size, MEM_DYNARRAY);
    if (!da->data)
    {
        FREE(da, sizeof(darray_t), MEM_DYNARRAY);
        return NULL;
    }

    da->size = 0;
    da->capacity = init_capacity;
    da->element_size = element_size;
    return da;
}

u64 da_append(darray_t *da, const void *element)
{
    if (da->size >= da->capacity)
    {
        u64 new_capacity = da->capacity * 2;
        void *new_data = ALLOC(new_capacity * da->element_size, MEM_DYNARRAY);
        if (!new_data) return 0;

        memcpy(new_data, da->data, da->size * da->element_size);

        FREE(da->data, da->capacity * da->element_size, MEM_DYNARRAY);
        da->data = new_data;
        da->capacity = new_capacity;
    }

    void *target = (char *)da->data + (da->size * da->element_size);
    memcpy(target, element, da->element_size);
    da->size++;
    return 1;
}

void *da_get(const darray_t *da, u64 index)
{
    if (index >= da->size) return NULL;
    return (char *)da->data + (index * da->element_size);
}

u64 da_set(darray_t *da, u64 index, const void *element)
{
    if (index >= da->size) return 0;
    void *target = (char *)da->data + (index * da->element_size);
    memcpy(target, element, da->element_size);
    return 1;
}

void da_free(darray_t *da)
{
    if (da)
    {
        FREE(da->data, da->capacity * da->element_size, MEM_DYNARRAY);
        FREE(da, sizeof(darray_t), MEM_DYNARRAY);
    }
}

u64 da_size(const darray_t *da) { return da->size; }

u64 da_capacity(const darray_t *da) { return da->capacity; }
