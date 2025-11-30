#ifndef SCENE_H
#define SCENE_H

#include "engine/core/define.h" // IWYU pragma: keep
#include "registry.h"
#include "engine/module/shader.h"
#include "engine/module/lights.h"
#include "engine/rendering/renderer.h"

void scene_system_render(registry_t *reg, render_system_t *rs,
                         shader_system_t *sh, lighting_system_t *ls);

#endif // SCENE_H
