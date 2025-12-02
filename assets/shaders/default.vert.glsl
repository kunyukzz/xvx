// VERTEX SHADER
#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 model;

layout(std140) uniform camera_block {
    mat4 proj;
    mat4 view;
};

out vec3 v_normal;
out vec2 v_texcoord;

void main() {
	v_normal = a_normal;

	v_texcoord = a_texcoord;

	gl_Position = proj * view * model * vec4(a_pos, 1.0);
}
