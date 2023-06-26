#include "pistacchio/log.hh"
#include "pistacchio/filesystem/obj.hh"
#include <deque>
#include <fstream>
#include <sstream>

static auto _log = Log("Filesystem: OBJ");

std::deque<std::string> string_split(std::string s, std::string l)
{
	std::deque<std::string> bucket;

	while (!s.empty()) {
		bucket.push_back(s.substr(0, s.find(l)));
		s.erase(0, bucket.back().length() + l.length());
	}

	return bucket;
}

std::string OBJ::to_string(const OBJ::Vertex& o)
{
	std::stringstream ss;

	ss << "(x: " << o.x << ", y: " << o.y << ", z: " << o.z << ", [w: " << o.w << "])";

	return ss.str();
}

std::string OBJ::to_string(const OBJ::Texture& o)
{
	std::stringstream ss;

	ss << "(u: " << o.u << ", [v: " << o.v << "], [w: " << o.w << "])";

	return ss.str();
}

std::string OBJ::to_string(const OBJ::Normal& o)
{
	std::stringstream ss;

	ss << "(x: " << o.x << ", y: " << o.y << ", z: " << o.z << ")";

	return ss.str();
}

std::string OBJ::to_string(const OBJ::Face::Indices& o)
{
	std::stringstream ss;

	ss << "(v: " << o.v << ", [vt: " << o.vt << "], [vn: " << o.vn << "])";

	return ss.str();
}

std::string OBJ::to_string(const OBJ::Face& o)
{
	std::stringstream ss;

	for (uint32_t i = 0; i < o.indices.size(); ++i) {
		auto& index = o.indices[i];

		ss << to_string(index);

		if (i != o.indices.size() - 1)
			ss << ", ";
	}

	return ss.str();
}

OBJ OBJ::load(const std::string &path)
{
	OBJ obj;

	std::ifstream file(path);

	if (!file) {
		_log.warn("Unable to find file " + path);
		return obj;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line[0] == '#') {
			// _log.debug("Comment: " + line);
			continue;
		} else if (line.empty()) {
			continue;
		} else {
			auto split = string_split(line, " ");

			auto token = split[0];
			split.pop_front();

			if (token == "v") {
				OBJ::Vertex v;

				for (uint32_t i = 0; i < split.size(); ++i)
					v.values[i] = std::stof(split[i]);

				obj.vertices.push_back(v);

				// _log.debug("Vertex: " + to_string(v));
			} else if (token == "vt") {
				OBJ::Texture vt;

				for (uint32_t i = 0; i < split.size(); ++i)
					vt.values[i] = std::stof(split[i]);

				obj.texture_coordinates.push_back(vt);

				// _log.debug("Texture: " + to_string(vt));
			} else if (token == "vn") {
				OBJ::Normal vn;

				for (uint32_t i = 0; i < split.size(); ++i) {
					vn.values[i] = std::stof(split[i]);
				}

				obj.vertex_normals.push_back(vn);

				// _log.debug("Normal: " + to_string(vn));
			} else if (token == "f") {
				OBJ::Face f;

				for (const auto& s : split) {
					OBJ::Face::Indices idx;

					auto idx_split = string_split(s, "/");

					for (uint32_t i = 0; i < idx_split.size(); ++i) {
						if (idx_split[i].empty())
							continue;

						idx.values[i] = std::stoi(idx_split[i]);
					}

					f.indices.push_back(idx);
				}

				obj.faces.push_back(f);

				// _log.debug("Face: " + to_string(f));
			} else {
				// _log.debug("Unhandled type '" + token + "'");
				continue;
			}
		}
	}

	return obj;
}
