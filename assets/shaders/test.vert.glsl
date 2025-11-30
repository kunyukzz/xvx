// VERTEX SHADER
#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 model;

layout(std140) uniform camera_block{
    mat4 proj;
    mat4 view;
};

out vec3 normal_pos;
out vec3 frag_pos;
out vec2 texcoord;

void main() {
	frag_pos = vec3(model * vec4(a_pos, 1.00));
	normal_pos = mat3(transpose(inverse(model))) * a_normal;
	texcoord = a_texcoord;

	gl_Position = proj * view * vec4(a_pos, 1.0);
}
