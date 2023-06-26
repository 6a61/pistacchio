#pragma once

#include <string>
#include <unordered_map>

#include <glad/gl.h>

class ShaderGL {
public:
	struct Attribute {
		int32_t size;
		uint32_t type;
		std::string name;
		uint32_t location;
	};
private:
	uint32_t m_name;
	uint32_t m_vao;
	std::unordered_map<std::string, Attribute> m_attributes;
	std::unordered_map<std::string, uint32_t> m_uniforms;

	uint32_t m_last_binding_index;
	std::unordered_map<uint32_t /* index */, uint32_t /* vbo */> m_bindings;
public:
	static constexpr auto VERTEX = GL_VERTEX_SHADER;
	static constexpr auto FRAGMENT = GL_FRAGMENT_SHADER;

	using AttributesMap = std::unordered_map<std::string, Attribute>;
	using UniformsMap = std::unordered_map<std::string, uint32_t>;

	ShaderGL(const std::unordered_map<uint32_t /* type */, std::string /* path */>& shaders);

	uint32_t id() const;
	uint32_t vao() const;
	AttributesMap attributes() const;
	UniformsMap uniforms() const;

	template<class Ty>
	void uniform(const char* uniform, const Ty& value);

	void bind_buffer(uint32_t binding_point, uint32_t vbo, void* offset, uint32_t stride);
	void bind_attribute(const char* attribute, uint32_t binding_point);
	void attribute_format(const char* attribute, uint32_t offset);
};
