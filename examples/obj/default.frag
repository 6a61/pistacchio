#version 450 core

in vec3 frag_position;
in vec3 frag_normal;
in flat vec3 frag_normal_flat;

uniform vec3  light_color;
uniform vec3  light_position;
uniform vec3  object_color;
uniform float ambient_strength;
uniform bool flat_shading;
uniform float model_opacity;
uniform float specular_strength;
uniform int specular_shininess;
uniform vec3 view_position;

out vec4 color;

void main() {
	vec3 normal = (flat_shading) ? frag_normal_flat : frag_normal;
	vec3 light_direction = normalize(light_position - frag_position);
	vec3 view_direction = normalize(view_position - frag_position);
	vec3 reflect_direction = reflect(-light_direction, normal);

	float diffuse_impact = max(dot(normal, light_direction), 0.0);
	float specular_impact = pow(max(dot(view_direction, reflect_direction), 0.0), specular_shininess);

	vec3 diffuse = diffuse_impact * light_color;
	vec3 ambient = ambient_strength * light_color;
	vec3 specular = specular_strength * specular_impact * light_color;

	color = vec4((ambient + diffuse + specular) * object_color, model_opacity);
}
