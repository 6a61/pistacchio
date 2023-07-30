#include "heightmap.hh"

Heightmap Heightmap::load(const std::string& path)
{
	int width            = 0;
	int height           = 0;
	int channels         = 0;
	int desired_channels = 1;

	u8* data = stbi_load(path.c_str(), &width, &height, &channels, desired_channels);

	std::vector<float> map;

	for (int i = 0; i < width * height; ++i)
		map.push_back(data[i] / 255.f);

	stbi_image_free(data);

	return Heightmap{
		.width = width,
		.height = height,
		.data = map
	};
}

Heightmap::Mesh Heightmap::mesh() const
{
	Mesh result;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			result.vertices.push_back({ x, data[y * width + x], y });

			auto normal = glm::vec3{ x, data[y * width + x], y };

			if (y != 0)
				normal += glm::vec3{ x, data[(y-1) * width + x], y-1 };
			if (y != height-1)
				normal += glm::vec3{ x, data[(y+1) * width + x], y+1 };
			if (x != 0)
				normal += glm::vec3{ x-1, data[y * width + (x-1)], y };
			if (x != width-1)
				normal += glm::vec3{ x+1, data[y * width + (x+1)], y };
			if (x != 0 && y != 0)
				normal += glm::vec3{ x-1, data[(y-1) * width + (x-1)], y};
			if (x != width-1 && y != height-1)
				normal += glm::vec3{ x+1, data[(y+1) * width + (x+1)], y};
			if (x != 0 && y != height-1)
				normal += glm::vec3{ x-1, data[(y+1) * width + (x-1)], y+1};
			if (x != width-1 && y != 0)
				normal += glm::vec3{ x+1, data[(y-1) * width + (x+1)], y-1};

			result.normals.push_back(glm::normalize(normal));
		}
	}

	for (int y = 0; y < height-1; ++y) {
		for (int x = 0; x < width-1; ++x) {
			result.indices.push_back( y * width + x );
			result.indices.push_back( (y+1) * width + x );
			result.indices.push_back( y * width + (x+1) );

			result.indices.push_back( (y+1) * width + x );
			result.indices.push_back( (y+1) * width + (x+1) );
			result.indices.push_back( y * width + (x+1) );
		}
	}

	return result;
}

float Heightmap::get(int x, int y) const
{
	return data[y * width + x] / 255.f;
}
