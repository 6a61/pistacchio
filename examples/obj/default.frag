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

out vec4 color;

void main() {
	vec3 normal = (flat_shading) ? frag_normal_flat : frag_normal;

	vec3 ambient = ambient_strength * light_color;

	vec3 light_direction = normalize(light_position - frag_position);
	float diffuse_impact = max(dot(normal, light_direction), 0.0);
	vec3 diffuse = diffuse_impact * light_color;

	color = vec4((ambient + diffuse) * object_color, model_opacity);
}
