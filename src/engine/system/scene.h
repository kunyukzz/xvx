#ifndef SCENE_H
#define SCENE_H

#include "engine/core/define.h" // IWYU pragma: keep
#include "registry.h"
#include "engine/module/material.h"
#include "engine/module/mesh.h"
#include "engine/rendering/renderer.h"

void scene_system_render(registry_t *reg, render_system_t *rs,
                         mesh_system_t *ms, material_system_t *mat);

#endif // SCENE_H
