#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include <glad/gl.h>
#include <stb_image.h>
#include "pistacchio/gl/texture.hh"

// This creates an invalid texture
TextureGL::TextureGL() : m_name(0), m_width(0), m_height(0)
{}

TextureGL::TextureGL(const std::string& path)
{
	int width = 0;
	int height = 0;
	int channels = 0;
	int desired_channels = 0;

	uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, desired_channels);

	if (data) {
		glCreateTextures(GL_TEXTURE_2D, 1, &m_name);

		glTextureParameteri(m_name, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_name, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureStorage2D(m_name, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(m_name, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	m_width = width;
	m_height = height;

	stbi_image_free(data);
}

TextureGL::TextureGL(uint8_t* data, int width, int height) :
	m_width(width),
	m_height(height)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_name);

	glTextureParameteri(m_name, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_name, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureStorage2D(m_name, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(m_name, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

uint32_t TextureGL::id()
{
	return m_name;
}

uint32_t TextureGL::width()
{
	return m_width;
}

uint32_t TextureGL::height()
{
	return m_height;
}
