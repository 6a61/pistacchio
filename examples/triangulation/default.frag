#version 450 core

in vec3 frag_normal;
in vec3 frag_position;

out vec4 color;

uniform bool shading;

void main() {
	float ambient_strength = 0.2;
	vec3 light_color = vec3(1.0, 1.0, 1.0);
	vec3 light_position = vec3(0.0, 3.0, 3.0);

	vec3 ambient = ambient_strength * light_color;

	vec3 light_direction = normalize(light_position - frag_position);
	float diffuse_impact = max(dot(frag_normal, light_direction), 0.0);
	vec3 diffuse = diffuse_impact * light_color;

	if (shading)
		color = vec4((ambient + diffuse) * vec3(1.0, 1.0, 1.0), 1.0);
	else
		color = vec4(1.0, 1.0, 1.0, 1.0);
}
