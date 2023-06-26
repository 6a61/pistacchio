#pragma once

#include <map>
#include <string>

// Loads FreeType font and creates bitmap atlas.
// FIXME: Current implementation may return an atlas with a width bigger than
// 16384 which is a common limit on GPU texture sizes...

class Font {
public:
	struct Glyph {
		uint32_t width;
		uint32_t height;

		// Horizontal offset (px) in atlas
		uint32_t offset;

		// How much to move the cursor after writing the glyph
		int32_t advance_x;
	};

	using GlyphMap = std::map<uint32_t, Glyph>;

	inline static const Glyph invalid_glyph = Glyph{
		.width = 0,
		.height = 0,
		.offset = 0,
		.advance_x = 0
	};
private:
	int32_t m_atlas_width;
	int32_t m_atlas_height;
	GlyphMap m_glyph_map;
	uint32_t* m_buffer;
public:
	Font(const std::string& path, int size);
	~Font();

	// Returns buffer containing bitmap atlas.
	uint32_t* buffer();

	// Atlas width in pixels
	int32_t width();

	// Atlas height in pixels
	int32_t height();

	//
	Glyph get(uint32_t char_code);
};
