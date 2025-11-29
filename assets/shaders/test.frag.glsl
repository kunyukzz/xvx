// FRAGMENT SHADER
#version 330 core
out vec4 frag_color;

in vec3 v_normal;
//in vec2 v_texcoord;

/*
uniform vec4 u_base_color;
uniform float u_metallic;
uniform float u_roughness;
uniform sampler2D u_texture;

uniform vec3 u_light_dir;
uniform vec3 u_light_color;
uniform vec3 u_ambient;
*/

void main() {
	/*
	vec4 albedo = texture(u_texture, v_texcoord);
    float diff = max(dot(v_normal, -u_light_dir), 0.0);
    vec3 lighting = u_ambient + (u_light_color * diff);
    vec3 final_color = albedo.rgb * u_base_color.rgb * lighting;
    frag_color = vec4(final_color, albedo.a);
	*/

	frag_color = vec4(0.5, 0.5, 0.5, 1.0);
}
