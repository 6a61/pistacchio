#include <array>
#include <fstream>
#include <sstream>

#include <glad/gl.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "pistacchio/gl/shader.hh"
#include "pistacchio/log.hh"

static auto _log = Log("Shader GL");

int32_t status(uint32_t shader)
{
	int32_t status;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	return status;
}

ShaderGL::ShaderGL(const std::unordered_map<uint32_t, std::string>& shaders) :
	m_last_binding_index(0)
{
	uint32_t program = glCreateProgram();

	uint32_t vao = 0;
	glCreateVertexArrays(1, &vao);

	for (const auto& [stage, path] : shaders) {
		uint32_t shader = glCreateShader(stage);

		std::ifstream file(path);

		if (!file.is_open())
			_log.warn("Unable to open " + path);

		std::stringstream buffer;

		buffer << file.rdbuf();

		std::string source_str(buffer.str());
		const char* source = source_str.c_str();

		glShaderSource(shader, 1, &source, nullptr);

		glCompileShader(shader);

		if (status(shader)) {
			glAttachShader(program, shader);
			glDeleteShader(shader);
		} else {
			int32_t error_length;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_length);

			std::string error;
			error.resize(error_length);

			glGetShaderInfoLog(shader, error_length, nullptr, error.data());

			_log.warn("Unable to compile shader " + path);

			printf("\n\t");
			for (auto c : error) {
				if (c == '\n')
					printf("\n\t");
				else
					printf("%c", c);
			}
			printf("\n");
		}
	}

	glLinkProgram(program);

	int32_t attribute_count = 0;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attribute_count);

	if (attribute_count > 0) {
		int32_t max_length = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);

		for (auto i = 0; i < attribute_count; ++i) {
			std::string name;
			int32_t size;
			GLenum type;
			int32_t length;

			name.resize(max_length, '\0');
			glGetActiveAttrib(program, i, max_length, &length, &size, &type, name.data());
			name.resize(length);

			uint32_t location = glGetAttribLocation(program, name.c_str());

			m_attributes.emplace(name, Attribute{
				.size = size,
				.type = type,
				.name = name,
				.location = location
			});

			glEnableVertexArrayAttrib(vao, i);
		}
	}

	int32_t uniform_count = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

	if (uniform_count > 0) {
		int32_t max_length = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);

		for (auto i = 0; i < uniform_count; ++i) {
			std::string name;
			int32_t size;
			GLenum type;
			int32_t length;

			name.resize(max_length, '\0');
			glGetActiveUniform(program, i, max_length, &length, &size, &type, name.data());
			name.resize(length);

			uint32_t location = glGetUniformLocation(program, name.c_str());
			m_uniforms.emplace(name, location);
		}
	}

	m_name = program;
	m_vao = vao;
}

uint32_t ShaderGL::id() const
{
	return m_name;
}

uint32_t ShaderGL::vao() const
{
	return m_vao;
}

ShaderGL::AttributesMap ShaderGL::attributes() const
{
	return m_attributes;
}

ShaderGL::UniformsMap ShaderGL::uniforms() const
{
	return m_uniforms;
}

template<>
void ShaderGL::uniform(const char* uniform, const int& value)
{
	if (!m_uniforms.contains(uniform))
		return;

	glProgramUniform1i(m_name, m_uniforms[uniform], value);
}

template<>
void ShaderGL::uniform(const char* uniform, const bool& value)
{
	if (!m_uniforms.contains(uniform))
		return;

	glProgramUniform1i(m_name, m_uniforms[uniform], value);
}

template<>
void ShaderGL::uniform(const char* uniform, const float& value)
{
	if (!m_uniforms.contains(uniform))
		return;

	glProgramUniform1f(m_name, m_uniforms[uniform], value);
}


template<>
void ShaderGL::uniform(const char* uniform, const std::array<float, 4>& value)
{
	if (!m_uniforms.contains(uniform))
		return;

	glProgramUniform4fv(m_name, m_uniforms[uniform], 1, value.data());
}

template<>
void ShaderGL::uniform(const char* uniform, const glm::vec3& value)
{
	if (!m_uniforms.contains(uniform)) {
		return;
	}

	glProgramUniform3fv(m_name, m_uniforms[uniform], 1, glm::value_ptr(value));
}

template<>
void ShaderGL::uniform(const char* uniform, const glm::mat4& value)
{
	if (!m_uniforms.contains(uniform)) {
		return;
	}

	glProgramUniformMatrix4fv(m_name, m_uniforms[uniform], 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderGL::bind_buffer(uint32_t binding_point, uint32_t vbo, void *offset, uint32_t stride)
{
	glVertexArrayVertexBuffer(m_vao, binding_point, vbo, (GLintptr)offset, stride);
}

void ShaderGL::bind_attribute(const char *attribute, uint32_t binding_point)
{
	if (!m_attributes.contains(attribute))
		return;

	glVertexArrayAttribBinding(m_vao, m_attributes[attribute].location, binding_point);
}

void ShaderGL::attribute_format(const char *attribute, uint32_t offset)
{
	uint32_t type = 0;
	uint32_t size = 0;

	auto attr = m_attributes[attribute];

	switch (m_attributes[attribute].type) {
	case GL_FLOAT_VEC2:
		type = GL_FLOAT;
		size = 2;
		break;
	case GL_FLOAT_VEC3:
		type = GL_FLOAT;
		size = 3;
		break;
	case GL_FLOAT_VEC4:
		type = GL_FLOAT;
		size = 4;
		break;
	default:
		_log.debug("Attribute " + std::string(attribute) + ": Unknown type " + std::to_string(attr.type));
	}

	glVertexArrayAttribFormat(m_vao, m_attributes[attribute].location, size, type, GL_FALSE, offset);
}
