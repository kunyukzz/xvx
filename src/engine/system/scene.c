#include "scene.h"
#include "engine/math/maths.h"
#include "engine/rendering/renderer.h"

void scene_system_render(registry_t *reg, render_system_t *rs,
                         shader_system_t *sh, lighting_system_t *ls)
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

        // NOTE: use render_draw if want direct call. (disable push & flush)
        // render_draw(rs, m->mesh);

        render_push(rs, m->mesh, m->texture);
    }

    render_flush(rs);
}

