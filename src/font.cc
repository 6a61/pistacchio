#include <functional>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "pistacchio/font.hh"
#include "pistacchio/log.hh"

static auto _log = Log("Core: Font");

void for_each_glyph(FT_Face face, FT_Int32 load_flags, std::function<void(uint32_t, FT_GlyphSlot)> callback)
{
	uint32_t glyph_index = 0;
	uint32_t char_code = FT_Get_First_Char(face, &glyph_index);

	while (glyph_index != 0) {
		// Limit characters to Unicode (0x0000-0x007F)
		// if (char_code <= 0x7F) {
		if (FT_Load_Glyph(face, glyph_index, load_flags) == 0) {
			callback(char_code, face->glyph);
		}
		// }

		char_code = FT_Get_Next_Char(face, char_code, &glyph_index);
	}
}

Font::Font(const std::string& path, int size)
{
	FT_Library library;
	FT_Face face;

	if (FT_Init_FreeType(&library) != 0)
		_log.warn("Unable to initialize Freetype");

	if (FT_New_Face(library, path.c_str(), 0, &face) != 0) {
		_log.warn("Unable to open font");
		FT_Done_FreeType(library);
	}

	FT_Set_Pixel_Sizes(face, 0, size);

	// Calculate atlas size and build character map

	int32_t max_ascent = 0;
	int32_t max_descent = 0;
	uint32_t atlas_width = 0;
	GlyphMap glyph_map;

	for_each_glyph(face, FT_LOAD_BITMAP_METRICS_ONLY,
	[&atlas_width, &max_ascent, &max_descent, &glyph_map](uint32_t char_code, FT_GlyphSlot glyph) -> void {
		if (glyph->bitmap_top > max_ascent)
			max_ascent = glyph->bitmap_top;

		int32_t glyph_height = glyph->metrics.height >> 6;

		if (glyph_height - glyph->bitmap_top > max_descent)
			max_descent = glyph_height - glyph->bitmap_top;

		glyph_map.emplace(char_code, Glyph{
			.width = glyph->bitmap.width,
			.height = glyph->bitmap.rows,
			.offset = atlas_width,
			.advance_x = glyph->advance.x >> 6,
		});

		atlas_width += glyph->bitmap.width;
	});

	// Populate atlas

	auto* atlas_buffer = new uint32_t[atlas_width * (max_ascent + max_descent)]();

	for_each_glyph(face, FT_LOAD_RENDER,
	[&atlas_buffer, &max_ascent, &atlas_width, &glyph_map](uint32_t char_code, FT_GlyphSlot glyph) -> void {
		// FIXME: Currently ignoring glyph->bitmap.pixelmode, assuming
		// FT_PIXEL_MODE_GRAY.

		uint32_t* buffer = new uint32_t[glyph->bitmap.width * glyph->bitmap.rows]();

		for (uint32_t i = 0; i < glyph->bitmap.width * glyph->bitmap.rows; ++i) {
			uint8_t alpha = glyph->bitmap.buffer[i];

			uint32_t rgba = (alpha << (8 * 3)) |
			                (0xFF << (8 * 2)) |
			                (0xFF << (8 * 1)) |
			                (0xFF << (8 * 0));

			buffer[i] = rgba;
		}

		for (uint32_t y = 0; y < glyph->bitmap.rows; ++y) {
			memcpy((uint32_t*)atlas_buffer + (y + max_ascent - glyph->bitmap_top) * atlas_width + glyph_map[char_code].offset,
			       &buffer[y * glyph->bitmap.width],
			       glyph->bitmap.width * 4);
		}

		delete[] buffer;
	});

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	m_atlas_width = atlas_width;
	m_atlas_height = max_ascent + max_descent;
	m_buffer = atlas_buffer;
	m_glyph_map = glyph_map;
}

Font::~Font()
{
	if (m_buffer)
		delete[] m_buffer;
}

uint32_t* Font::buffer()
{
	return m_buffer;
}

int32_t Font::width()
{
	return m_atlas_width;
}

int32_t Font::height()
{
	return m_atlas_height;
}

Font::Glyph Font::get(uint32_t char_code)
{
	if (m_glyph_map.contains(char_code))
		return m_glyph_map[char_code];

	return Font::invalid_glyph;
}
