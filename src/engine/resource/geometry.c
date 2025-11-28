#include "geometry.h"
#include "engine/core/memory.h"
#include "engine/math/math_types.h"

/*
geometry_t geometry_gen_cube(f32 size)
{
    if (size < 1.0f) size = 1.0f;

    geometry_t geo = {};

    geo.vert_size = sizeof(vertex);
    geo.indices_size = sizeof(u32);
    geo.vert_count = 4;
    geo.indices_count = 6;

    geo.vertices = ALLOC(sizeof(vertex) * geo.vert_count, MEM_ARRAY);
    geo.indices = ALLOC(sizeof(u32) * geo.indices_count, MEM_ARRAY);

    f32 half_size = size * 0.5f;

    vertex *vert = (vertex *)geo.vertices;
    u32 *indcs = (u32 *)geo.indices;

    vec3 normal_front = (vec3){{0.0f, 0.0f, 1.0f}};
    vec2 uv_01 = (vec2){{0.0f, 1.0f}};

    vert[0] = (vertex){.position = {{-half_size, -half_size, half_size}},
                       .normal = normal_front,
                       .texcoord = uv_01};

    // clang-format off
    // front face
    indcs[0] = 0; indcs[1] = 1; indcs[2] = 2; indcs[3] = 2; indcs[4] = 3;
indcs[5] = 0;

    return geo;
}
*/

geometry_t geometry_gen_plane(f32 width, f32 height, u32 segments)
{
    if (segments < 1) segments = 1;

    geometry_t geo = {};
    geo.vert_size = sizeof(vertex);
    geo.indices_size = sizeof(u32);

    u32 verts_x = segments + 1;
    u32 verts_z = segments + 1;

    geo.vert_count = verts_x * verts_z;
    geo.indices_count = segments * segments * 6;

    geo.vertices = ALLOC(sizeof(vertex) * geo.vert_count, MEM_ARRAY);
    geo.indices = ALLOC(sizeof(u32) * geo.indices_count, MEM_ARRAY);

    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float dx = width / (float)segments;
    float dz = height / (float)segments;

    vertex *verts = (vertex *)geo.vertices;
    u32 *inds = (u32 *)geo.indices;

    u32 v = 0;
    for (u32 z = 0; z < verts_z; ++z)
    {
        for (u32 x = 0; x < verts_x; ++x)
        {
            float px = -hw + (float)x * dx;
            float pz = -hh + (float)z * dz;

            verts[v].position = (vec3){{px, 0.0f, -pz}};
            verts[v].normal = (vec3){{0.0f, 1.0f, 0.0f}};
            verts[v].texcoord = (vec2){
                {(float)x / (float)segments, (float)z / (float)segments}};
            v++;
        }
    }

    u32 i = 0;
    for (u32 z = 0; z < segments; ++z)
    {
        for (u32 x = 0; x < segments; ++x)
        {
            u32 i0 = z * verts_x + x;
            u32 i1 = i0 + 1;
            u32 i2 = i0 + verts_x;
            u32 i3 = i2 + 1;

            inds[i++] = i0;
            inds[i++] = i1;
            inds[i++] = i3;
            inds[i++] = i0;
            inds[i++] = i3;
            inds[i++] = i2;
        }
    }

    return geo;
}

geometry_t geometry_gen_cube(float width, float height, float depth)
{
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    if (depth < 1) depth = 1;

    geometry_t geo = {};
    geo.vert_size = sizeof(vertex);
    geo.indices_size = sizeof(u32);

    geo.vert_count = 24;
    geo.indices_count = 36;

    geo.vertices = ALLOC(sizeof(vertex) * geo.vert_count, MEM_ARRAY);
    geo.indices = ALLOC(sizeof(u32) * geo.indices_count, MEM_ARRAY);

    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float hd = depth * 0.5f;

    vertex *verts = (vertex *)geo.vertices;
    u32 *inds = (u32 *)geo.indices;

    vec3 corners[8] = {
        {{-hw, -hh, -hd}}, // 0: left bottom back
        {{hw, -hh, -hd}},  // 1: right bottom back
        {{hw, hh, -hd}},   // 2: right top back
        {{-hw, hh, -hd}},  // 3: left top back
        {{-hw, -hh, hd}},  // 4: left bottom front
        {{hw, -hh, hd}},   // 5: right bottom front
        {{hw, hh, hd}},    // 6: right top front
        {{-hw, hh, hd}}    // 7: left top front
    };

    vec3 normal_front = {{0.0f, 0.0f, 1.0f}};
    vec3 normal_back = {{0.0f, 0.0f, -1.0f}};
    vec3 normal_right = {{1.0f, 0.0f, 0.0f}};
    vec3 normal_left = {{-1.0f, 0.0f, 0.0f}};
    vec3 normal_top = {{0.0f, 1.0f, 0.0f}};
    vec3 normal_bottom = {{0.0f, -1.0f, 0.0f}};

    vec2 uv_bl = {{0.0f, 0.0f}};
    vec2 uv_br = {{1.0f, 0.0f}};
    vec2 uv_tl = {{0.0f, 1.0f}};
    vec2 uv_tr = {{1.0f, 1.0f}};

    // FRONT FACE
    verts[0] = (vertex){corners[4], normal_front, uv_bl};
    verts[1] = (vertex){corners[5], normal_front, uv_br};
    verts[2] = (vertex){corners[6], normal_front, uv_tr};
    verts[3] = (vertex){corners[7], normal_front, uv_tl};

    // BACK FACE
    verts[4] = (vertex){corners[1], normal_back, uv_bl};
    verts[5] = (vertex){corners[0], normal_back, uv_br};
    verts[6] = (vertex){corners[3], normal_back, uv_tr};
    verts[7] = (vertex){corners[2], normal_back, uv_tl};

    // RIGHT FACE
    verts[8] = (vertex){corners[5], normal_right, uv_bl};
    verts[9] = (vertex){corners[1], normal_right, uv_br};
    verts[10] = (vertex){corners[2], normal_right, uv_tr};
    verts[11] = (vertex){corners[6], normal_right, uv_tl};

    // LEFT FACE
    verts[12] = (vertex){corners[0], normal_left, uv_bl};
    verts[13] = (vertex){corners[4], normal_left, uv_br};
    verts[14] = (vertex){corners[7], normal_left, uv_tr};
    verts[15] = (vertex){corners[3], normal_left, uv_tl};

    // TOP FACE
    verts[16] = (vertex){corners[7], normal_top, uv_bl};
    verts[17] = (vertex){corners[6], normal_top, uv_br};
    verts[18] = (vertex){corners[2], normal_top, uv_tr};
    verts[19] = (vertex){corners[3], normal_top, uv_tl};

    // BOTTOM FACE
    verts[20] = (vertex){corners[0], normal_bottom, uv_bl};
    verts[21] = (vertex){corners[1], normal_bottom, uv_br};
    verts[22] = (vertex){corners[5], normal_bottom, uv_tr};
    verts[23] = (vertex){corners[4], normal_bottom, uv_tl};

    u32 face_indices[6][6] = {// Front face
                              {0, 1, 2, 2, 3, 0},
                              // Back face
                              {4, 5, 6, 6, 7, 4},
                              // Right face
                              {8, 9, 10, 10, 11, 8},
                              // Left face
                              {12, 13, 14, 14, 15, 12},
                              // Top face
                              {16, 17, 18, 18, 19, 16},
                              // Bottom face
                              {20, 21, 22, 22, 23, 20}};

    for (u32 i = 0; i < 6; i++)
    {
        for (u32 j = 0; j < 6; j++)
        {
            inds[i * 6 + j] = face_indices[i][j];
        }
    }

    return geo;
}

void geometry_destroy(geometry_t *geo)
{
    if (!geo) return;

    FREE(geo->vertices, sizeof(vertex) * geo->vert_count, MEM_ARRAY);
    FREE(geo->indices, sizeof(u32) * geo->indices_count, MEM_ARRAY);
    geo->vertices = NULL;
    geo->indices = NULL;
    geo->vert_count = 0;
    geo->indices_count = 0;
}
