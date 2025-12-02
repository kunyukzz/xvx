// FRAGMENT SHADER
#version 330 core
out vec4 frag_color;

in vec3 v_normal;
in vec2 v_texcoord;

uniform vec4 base_color;
uniform float metallic;
uniform float roughness;
uniform sampler2D object_texture;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 ambient;

void main() {
	vec4 albedo = texture(object_texture, v_texcoord);

	// diffuse
    float diff = max(dot(v_normal, -light_pos), 0.0);
    //float diff = 1.0;

    vec3 lighting = ambient + (light_color * diff);

    vec3 final_color = albedo.rgb * base_color.rgb * lighting;
    
    frag_color = vec4(final_color, albedo.a);
	//frag_color = texture(object_texture, v_texcoord);
    //frag_color = vec4(1.0);
}
