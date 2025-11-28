#ifndef SCENE_H
#define SCENE_H

#include "engine/core/define.h" // IWYU pragma: keep
#include "registry.h"
#include "engine/rendering/renderer.h"

void scene_system_render(registry_t *reg, render_system_t *re);

#endif // SCENE_H
