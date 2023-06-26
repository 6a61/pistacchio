#pragma once

#include <stdint.h>
#include <string>

class TextureGL {
private:
	uint32_t m_name;
	uint32_t m_width;
	uint32_t m_height;
public:
	TextureGL(); // This creates an invalid texture
	TextureGL(const std::string& path);
	TextureGL(uint8_t* data, int width, int height);

	uint32_t id();
	uint32_t width();
	uint32_t height();
};
