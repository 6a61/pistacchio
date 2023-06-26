#version 450 core

in vec3 vertex;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 frag_normal;
out vec3 frag_normal_flat;
out vec3 frag_position;

void main() {
	gl_Position = projection * view * model * vec4(vertex, 1.0);
	frag_normal = mat3(transpose(inverse(model))) * normal;
	frag_normal_flat = frag_normal;
	frag_position = vec3(model * vec4(vertex, 1.0));
}
