#include "scene.h"
#include "engine/math/maths.h"

#include <stdio.h>

void scene_system_render(registry_t *reg, render_system_t *rs,
                         mesh_system_t *ms, material_system_t *mat)
{
    for (u32 i = 0; i < reg->capacity; ++i)
    {
        // Just check if this slot has the required components
        component_type_t *mask = da_get(reg->component_mask, i);
        if (!(*mask & COMP_TRANSFORM) || !(*mask & COMP_MODEL)) continue;

        transform_comp_t *t = da_get(reg->transform, i);
        model_comp_t *m = da_get(reg->model, i);

        if (t->dirty)
        {
            t->matrix = mat4_transform(t->position, t->rotation, t->scale);
            t->dirty = false;
        }

        u32 mesh_idx = m->mesh & INVALID_16;
        u32 mat_idx = m->material & INVALID_16;

        mesh_t *mesh_o = &ms->meshes[mesh_idx];
        material_t *mat_o = &mat->materials[mat_idx];

        u64 sort = ((u64)mat_o->id << 32) | ((u64)mesh_o->id << 16) | i;

        render_push(rs, m->mesh, m->material, t->matrix, sort);
    }

    render_flush(rs);
}

