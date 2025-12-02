#include "app.h"
#include "memory.h"
#include "engine/math/maths.h"

#include "engine/system/scene.h"

b8 application_init(application_t *app)
{
    u64 estimated_memory = 1 * 1024 * 1024;
    if (!memory_sys_init(estimated_memory))
    {
        LOG_ERROR("Failed to init memory system with estimated size: %lu",
                  estimated_memory);
        return false;
    }

    arena_create(64 * 1024, &app->arena, NULL);

    app->fs = file_system_init(&app->arena);
    app->ws = window_sys_init(&app->arena, 1280, 720, "XVX Testbed");
    app->ip = input_sys_init(&app->arena);
    app->cs = camera_sys_init(&app->arena);
    app->rs = render_sys_init(&app->arena);
    app->sh = shader_sys_init(&app->arena, 8);
    app->ls = light_sys_init(&app->arena, 2);
    app->ms = mesh_sys_init(&app->arena, 8);
    app->ts = texture_sys_init(&app->arena, 8);
    app->mts = material_sys_init(&app->arena, 8);
    app->reg = registry_sys_init(&app->arena, 8);
    app->game = game_init();

#if DEBUG
    // LOG_DEBUG("--- Memory Addresses ---");
    // LOG_DEBUG("Filesystem: %p", app->fs);
    // LOG_DEBUG("Window:     %p", app->ws);
    // LOG_DEBUG("Input:      %p", app->ip);
    // LOG_DEBUG("Camera:     %p", app->cs);
    // LOG_DEBUG("Render:     %p", app->rs);
    // LOG_DEBUG("Shader:     %p", app->sh);
    // LOG_DEBUG("Light:      %p", app->ls);
    // LOG_DEBUG("Mesh:       %p", app->ms);
    // LOG_DEBUG("Texture:    %p", app->ts);
    // LOG_DEBUG("Material:   %p", app->mts);
    // LOG_DEBUG("Registry:   %p", app->reg);

    u64 used = arena_used(&app->arena);
    u64 total = app->arena.total_size;
    f32 usage_percent = (f32)used / (f32)total * 100.0f;
    LOG_TRACE("Arena Usage: %lu/%lu bytes (%.1f%%)", used, total,
              usage_percent);

    if (usage_percent > 90.0f)
    {
        LOG_WARN("Arena nearly full! %.1f%% used", usage_percent);
    }
#endif

    LOG_INFO("Engine Initialize");
    return true;
}

b8 application_run(application_t *app)
{
    const f64 TARGET_FPS = 60.0;
    const f64 TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

    b8 cap_fps = false;

    f64 fps_timer = 0.0;
    u32 fps_counter = 0;

    timer_start(&app->time);
    f64 prev = timer_get();

    LOG_INFO("%s", mem_debug_stat());

    while (!window_sys_close(app->ws))
    {
        f64 curr = timer_get();
        f64 delta = curr - prev;
        prev = curr;

        fps_timer += delta;
        fps_counter++;

        if (fps_timer >= 1.0)
        {
            f64 avg_delta = fps_timer / fps_counter;
            f64 ms = avg_delta * 1000.0;
            f64 fps = fps_counter / fps_timer;

            LOG_INFO("FPS: %.0f | Frame: %.2f ms", fps, ms);

            fps_counter = 0;
            fps_timer -= 1.0;
        }

        window_sys_poll(app->ws);
        input_sys_update(app->ip, delta);

        game_update(app->game, delta);
        game_render(app->game, delta);
        camera_update(app->cs);

        render_sys_begin(app->rs, WORLD_PASS);

        static f32 orbit = 0.0f;
        orbit += (f32)delta * 0.5f;

        light_t *def = light_get(app->ls->default_light);

        if (def)
        {
            f32 radius = 5.0f;
            def->position.x = m_sin(orbit) * radius;
            def->position.y = 2.0f;
            def->position.z = m_cos(orbit) * radius;
        }

        // vec3 light_pos = def->position;
        vec3 view_pos = app->cs->world.position;
        vec4 obj_color = (vec4){{1.0f, 1.0f, 1.0f, 1.0f}};
        vec3 light_color = (vec3){{1.0f, 1.0f, 1.0f}};
        vec3 ambient = (vec3){{0.5, 0.5, 0.5}};

        // bind shader
        shader_bind(0);
        // shader_set_model(0, mat4_identity());
        shader_set_base_color(0, obj_color);
        shader_set_lightpos(0, def->position);
        shader_set_lightcolor(0, light_color);
        // shader_set_viewpos(0, view_pos);
        shader_set_ambient(0, ambient);
        // shader_set_object_color(0, obj_color);
        shader_set_sampler(0, 0);

        // TODO: for now, scene only doing temporary thing to collect entity
        // change this to proper scene system
        scene_system_render(app->reg, app->rs, app->ms, app->mts);

        // TODO: simple rendergraph (?)

        render_sys_end(app->rs, WORLD_PASS);

        window_sys_swapbuffer(app->ws);

        // frame limiting
        if (cap_fps)
        {
            f64 frame_end = timer_get();
            f64 frame_dur = frame_end - curr;

            if (frame_dur < TARGET_FRAME_TIME)
            {
                f64 target_wake = curr + TARGET_FRAME_TIME;
                get_sleep(target_wake);
            }
        }
    }

    game_kill(app->game);

    registry_sys_kill(app->reg);

    material_sys_kill(app->mts);
    texture_sys_kill(app->ts);
    mesh_sys_kill(app->ms);

    light_sys_kill(app->ls);
    shader_sys_kill(app->sh);
    render_sys_kill(app->rs);
    camera_sys_kill(app->cs);

    input_sys_kill(app->ip);
    window_sys_kill(app->ws);
    file_system_kill(app->fs);

    arena_kill(&app->arena);
    memory_sys_kill();

    LOG_INFO("Engine Shutdown");
    return true;
}
