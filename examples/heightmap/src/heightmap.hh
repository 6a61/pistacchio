#pragma once

#include <glm/geometric.hpp>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <pistacchio/types.hh>
#include <stb_image.h>

struct Heightmap {
	struct Mesh {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<u32> indices;
	};

	int width;
	int height;
	std::vector<float> data;

	static Heightmap load(const std::string& path);
	Mesh mesh() const;
	float get(int x, int y) const;
};
