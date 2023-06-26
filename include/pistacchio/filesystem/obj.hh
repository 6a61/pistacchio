#pragma once

#include <array>
#include <string>
#include <vector>

//
// Specification: http://paulbourke.net/dataformats/obj/
//
struct OBJ {
	struct Vertex {
		std::array<float, 4> values = { 1.0f, 1.0f, 1.0f, 1.0f };

		float& x = values[0];
		float& y = values[1];
		float& z = values[2];
		float& w = values[3]; // Optional

		Vertex() = default;
		Vertex(const Vertex& v) : values { v.values } {};
	};

	struct Texture {
		std::array<float, 3> values = { 0 } ;

		float& u = values[0];
		float& v = values[1]; // Optional
		float& w = values[2]; // Optional

		Texture() = default;
		Texture(const Texture& t) : values{ t.values } {};
	};

	struct Normal {
		std::array<float, 3> values = { 0 };

		float& x = values[0];
		float& y = values[1];
		float& z = values[2];

		Normal() = default;
		Normal(const Normal& r) : values{ r.values } {};
	};

	struct Face {
		struct Indices {
			std::array<uint32_t, 3> values = { 0 };

			uint32_t& v  = values[0];
			uint32_t& vt = values[1]; // Optional
			uint32_t& vn = values[2]; // Optional

			Indices() = default;
			Indices(const Indices& r) : values{ r.values } {};
		};

		std::vector<Indices> indices;
	};

	std::vector<Vertex> vertices;
	std::vector<Texture> texture_coordinates;
	std::vector<Normal> vertex_normals;
	std::vector<Face> faces;

	static OBJ load(const std::string& path);
	static std::string to_string(const OBJ::Vertex& o);
	static std::string to_string(const OBJ::Texture& o);
	static std::string to_string(const OBJ::Normal& o);
	static std::string to_string(const OBJ::Face::Indices& o);
	static std::string to_string(const OBJ::Face& o);
};
