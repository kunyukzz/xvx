// FRAGMENT SHADER
#version 330 core
out vec4 frag_color;

in vec3 normal_pos;
in vec3 frag_pos;
in vec2 texcoord;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 view_pos;
uniform vec3 object_color;
uniform sampler2D object_texture;

void main() {
	vec4 albedo = texture(object_texture, texcoord);
	//vec3 light_dir = normalize(-light_pos);
	vec3 light_dir = normalize(light_pos - frag_pos);

	//ambient
	float ambient_strength = 0.3;
	vec3 ambient = ambient_strength * light_color;

	//diffuse
	vec3 norm = normalize(normal_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_color;

	//specular
	float specular_strength = 0.3;
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_strength * spec * light_color;

	//lighting + texture
	vec3 lighting = (ambient + diffuse + specular);
	vec3 result = lighting * albedo.rgb * object_color;
	frag_color = vec4(result, albedo.a);
	//frag_color = vec4(result, 1.0);
	//frag_color = vec4(0.5, 0.5, 0.5, 1.0);
}
