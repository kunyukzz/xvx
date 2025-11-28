#include "registry.h"
#include "engine/core/container/freelist.h"

#include <string.h>

static freelist_t *g_entt_freelist = NULL;

registry_t *registry_sys_init(arena_alloc_t *arena, u32 capacity)
{
    registry_t *reg = arena_alloc(arena, sizeof(registry_t));
    if (!reg) return NULL;

    reg->arena = arena;
    reg->capacity = capacity;

    reg->component_mask = da_create(capacity, sizeof(component_type_t));
    reg->transform = da_create(capacity, sizeof(transform_comp_t));
    reg->model = da_create(capacity, sizeof(model_comp_t));

    if (!reg->component_mask || !reg->transform || !reg->model)
    {
        LOG_WARN("failed to allocate registry array");
        da_free(reg->component_mask);
        da_free(reg->transform);
        da_free(reg->model);
        return NULL;
    }

    component_type_t def_comp = {0};
    transform_comp_t def_tr_comp = {0};
    model_comp_t def_mesh_comp = {0};

    for (u32 i = 0; i < capacity; ++i)
    {
        da_append(reg->component_mask, &def_comp);
        da_append(reg->transform, &def_tr_comp);
        da_append(reg->model, &def_mesh_comp);
    }

    g_entt_freelist = freelist_create(arena, capacity);
    if (!g_entt_freelist) return NULL;

    LOG_INFO("Registry Init");
    return reg;
}

void registry_sys_kill(registry_t *reg)
{
    if (!reg) return;

    freelist_destroy(g_entt_freelist);

    da_free(reg->component_mask);
    da_free(reg->transform);
    da_free(reg->model);

    memset(reg, 0, sizeof(registry_t));
    LOG_INFO("Registry Kill");
}

entity_id registry_create_entity(registry_t *reg)
{
    u32 index = freelist_alloc(g_entt_freelist);
    if (index == INVALID_32)
    {
        LOG_ERROR("No free entity slots available");
        return ENTITY_NULL;
    }

    static u32 next_gen = 1;
    entity_id entity = entity_create(index, next_gen++);

    component_type_t *mask = da_get(reg->component_mask, index);
    *mask = COMP_NONE;

    LOG_DEBUG("Created entity (index: %u, generation: %u)", entity,
              entity.generation);

    return entity;
}

void registry_destroy_entity(registry_t *reg, entity_id entity)
{
    if (!registry_is_entity_valid(reg, entity))
    {
        LOG_WARN("Attempted to destroy invalid entity");
        return;
    }

    u32 index = entity_get_index(entity);

    component_type_t *mask = da_get(reg->component_mask, index);
    *mask = COMP_NONE;

    memset(da_get(reg->transform, index), 0, sizeof(transform_comp_t));
    memset(da_get(reg->model, index), 0, sizeof(model_comp_t));

    freelist_free(g_entt_freelist, index);

    LOG_DEBUG("Freed entity index %u", index);
}

b8 registry_is_entity_valid(registry_t *reg, entity_id entity)
{
    u32 index = entity_get_index(entity);
    if (index >= reg->capacity) return false;

    component_type_t *mask = da_get(reg->component_mask, index);
    return *mask != COMP_NONE;
}

void registry_add_transform(registry_t *reg, entity_id entity,
                            transform_comp_t transform)
{
    u32 index = entity_get_index(entity);
    if (index >= reg->capacity)
    {
        LOG_ERROR("Entity index out of bounds: %u", index);
        return;
    }

    da_set(reg->transform, index, &transform);

    component_type_t *mask = da_get(reg->component_mask, index);
    *mask |= COMP_TRANSFORM;

    LOG_DEBUG("Added transform to entity %u", entity);
}

void registry_add_model(registry_t *reg, entity_id entity, model_comp_t model)
{
    u32 index = entity_get_index(entity);
    if (index >= reg->capacity)
    {
        LOG_ERROR("Entity index out of bounds: %u", index);
        return;
    }

    da_set(reg->model, index, &model);

    component_type_t *mask = da_get(reg->component_mask, index);
    *mask |= COMP_MODEL;
}

transform_comp_t *registry_get_transform(registry_t *reg, entity_id entity)
{
    if (!registry_has_transform(reg, entity)) return NULL;

    u32 index = entity_get_index(entity);
    return da_get(reg->transform, index);
}

b8 registry_has_transform(registry_t *reg, entity_id entity)
{
    if (!registry_is_entity_valid(reg, entity)) return false;

    u32 index = entity_get_index(entity);

    component_type_t *mask = da_get(reg->component_mask, index);
    return mask && (*mask & COMP_TRANSFORM);
}

model_comp_t *registry_get_model(registry_t *reg, entity_id entity)
{
    if (!registry_has_model(reg, entity)) return NULL;

    u32 index = entity_get_index(entity);
    return da_get(reg->model, index);
}

b8 registry_has_model(registry_t *reg, entity_id entity)
{
    if (!registry_is_entity_valid(reg, entity)) return false;

    u32 index = entity_get_index(entity);
    component_type_t *mask = da_get(reg->component_mask, index);
    return mask && (*mask & COMP_MODEL);
}

