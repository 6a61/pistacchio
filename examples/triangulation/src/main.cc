#include <algorithm>
#include <glm/geometric.hpp>
#include <set>
#include <vector>
#include "glad/gl.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "pistacchio/app.hh"
#include "pistacchio/filesystem/obj.hh"
#include "pistacchio/gl/shader.hh"
#include "pistacchio/gl/window.hh"
#include "pistacchio/input.hh"
#include "pistacchio/time.hh"
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "pistacchio/log.hh"
#include "pistacchio/types.hh"
#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullPoints.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullVertexSet.h>
#include <libqhullcpp/QhullFacetSet.h>

#define MC_IMPLEM_ENABLE
#include "marching_cubes.hh"

using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
using mat4 = glm::mat4;

static auto _log = Log("Triangulation");

class Delaunay : public App {
private:
	WindowGL window = WindowGL("Triangulation", Window::CENTERED, Window::CENTERED, 1280, 768);
	ShaderGL shader = ShaderGL({
		{ ShaderGL::VERTEX, "default.vert" },
		{ ShaderGL::FRAGMENT, "default.frag" }
	});

	enum RenderMode { SOLID, WIREFRAME, POINTS };
	enum Algorithm { DELAUNAY, MARCHING_CUBES };
	int render_mode = RenderMode::SOLID;
	int algorithm = Algorithm::DELAUNAY;
	int previous_algorithm = Algorithm::DELAUNAY;
	bool shading = true;
	bool show_camera_window = true;

	std::vector<vec3> normals;
	std::vector<uint32_t> indices;
	std::vector<vec3> sphere_cloud = sphere_points(200, 200, 1.0f);
	std::vector<float> sphere_field = scalar_field(sphere_cloud, ivec3{ 100, 100, 100 });
	double time_algorithm_start = Time::seconds();
	std::vector<vec3> sphere = delaunay(sphere_cloud);
	double time_algorithm_end = Time::seconds();

	vec3 camera_position = vec3{ 0.0f, 0.0f, -4.5f };
	vec3 camera_rotation = vec3{ 0.0f, 30.0f, 0.0f };

	u32 vao = GL_NONE;
	u32 buffer_vertices = GL_NONE;
	u32 buffer_normals = GL_NONE;
	u32 buffer_indices = GL_NONE;
public:
	Delaunay() : App(30.0, 120.0)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().IniFilename = nullptr;
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(window.sdl_window(), window.data());
		ImGui_ImplOpenGL3_Init("#version 450 core");

		// OpenGL initialization

		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_DEPTH_TEST);

		glCreateBuffers(1, &buffer_vertices);
		glNamedBufferData(
			buffer_vertices,
			sphere.size() * sizeof(float) * 3,
			sphere.data(),
			GL_DYNAMIC_DRAW);

		glCreateBuffers(1, &buffer_normals);
		glNamedBufferData(
			buffer_normals,
			normals.size() * sizeof(float) * 3,
			normals.data(),
			GL_DYNAMIC_DRAW
		);

		glCreateBuffers(1, &buffer_indices);
		glNamedBufferData(
			buffer_indices,
			indices.size() * sizeof(uint32_t),
			indices.data(),
			GL_DYNAMIC_DRAW
		);

		glCreateVertexArrays(1, &vao);
		glVertexArrayVertexBuffer(vao, 0, buffer_vertices, 0, sizeof(float) * 3);
		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 0, 0);

		glVertexArrayVertexBuffer(vao, 1, buffer_normals, 0, sizeof(float) * 3);
		glEnableVertexArrayAttrib(vao, 1);
		glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 1, 1);

		glVertexArrayElementBuffer(vao, buffer_indices);
	}
protected:
	void update(double dt) override
	{
		Input::update();

		for (const auto& event: Input::sdl()) {
			ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.type) {
			case SDL_QUIT:
				this->stop();
				break;
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Triangulation")) {
				if (ImGui::MenuItem("Delaunay", nullptr, (algorithm == Algorithm::DELAUNAY)))
					algorithm = Algorithm::DELAUNAY;

				if (ImGui::MenuItem("Marching Cubes", nullptr, (algorithm == Algorithm::MARCHING_CUBES)))
					algorithm = Algorithm::MARCHING_CUBES;

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					this->stop();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows")) {
				if (ImGui::MenuItem("Camera", nullptr, show_camera_window))
					show_camera_window = !show_camera_window;

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (show_camera_window) {
			if (ImGui::Begin("Camera", &show_camera_window, ImGuiWindowFlags_AlwaysAutoResize)) {
				/*
				const char* algorithm_items[] = { "Delaunay", "Marching Cubes" };

				if (ImGui::BeginCombo("Algorithm", algorithm_items[algorithm])) {
					for (int n = 0; n < 2; ++n) {
						if (ImGui::Selectable(algorithm_items[n], (algorithm == n)))
							algorithm = n;

						if (algorithm == n)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				*/

				ImGui::DragFloat3("Position##camposX", &camera_position[0], 0.05f, -10.0f, 10.0f);
				ImGui::DragFloat3("Rotation##camrotX", &camera_rotation[0], 1.0f, -360.0f, 360.0f);

				ImGui::RadioButton("Solid", &render_mode, SOLID); ImGui::SameLine();
				ImGui::RadioButton("Wireframe", &render_mode, WIREFRAME); ImGui::SameLine();
				ImGui::RadioButton("Points", &render_mode, POINTS);

				ImGui::Checkbox("Shading", &shading);

				ImGui::LabelText("Algorithm Time", "%.2f", (time_algorithm_end - time_algorithm_start));
			}; ImGui::End();
		}

		ImGui::EndFrame();

		if (algorithm != previous_algorithm) {
			previous_algorithm = algorithm;

			switch (algorithm) {
				case DELAUNAY:
					time_algorithm_start = Time::seconds();
					sphere = delaunay(sphere_cloud);
					time_algorithm_end = Time::seconds();

					glNamedBufferData(
						buffer_vertices,
						sphere.size() * sizeof(float) * 3,
						sphere.data(),
						GL_DYNAMIC_DRAW);

					glNamedBufferData(
						buffer_normals,
						normals.size() * sizeof(float) * 3,
						normals.data(),
						GL_DYNAMIC_DRAW
					);
					break;
				case MARCHING_CUBES:
					time_algorithm_start = Time::seconds();
					sphere = marching_cubes(sphere_field, ivec3{ 100, 100, 100 });
					time_algorithm_end = Time::seconds();

					glNamedBufferData(
						buffer_vertices,
						sphere.size() * sizeof(float) * 3,
						sphere.data(),
						GL_DYNAMIC_DRAW);

					glNamedBufferData(
						buffer_indices,
						indices.size() * sizeof(uint32_t),
						indices.data(),
						GL_DYNAMIC_DRAW
					);

					glNamedBufferData(
						buffer_normals,
						normals.size() * sizeof(float) * 3,
						normals.data(),
						GL_DYNAMIC_DRAW
					);
	 				break;
			}
		}
	}

	void render(double alpha) override
	{
		float clear_color[4] = { 0.33f, 0.33f, 0.33f, 0.33f };
		glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);
		float clear_depth = 1.0;
		glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clear_depth);

		mat4 projection = glm::perspective(glm::radians(45.0f), float(window.width()) / float(window.height()), 0.1f, 100.0f);
		mat4 model = glm::mat4(1.0f);
		mat4 view = glm::mat4(1.0f);

		view = glm::translate(view, camera_position);
		view = glm::rotate(view, glm::radians(camera_rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(camera_rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(camera_rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

		shader.uniform("projection", projection);
		shader.uniform("model", model);
		shader.uniform("view", view);
		shader.uniform("shading", shading);

		glUseProgram(shader.id());
		glBindVertexArray(vao);

		switch (render_mode) {
		case SOLID:
			glNamedBufferData(
				buffer_vertices,
				sphere.size() * sizeof(float) * 3,
				sphere.data(),
				GL_DYNAMIC_DRAW);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (algorithm == Algorithm::DELAUNAY)
				glDrawArrays(GL_TRIANGLES, 0, sphere.size());
			else
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			break;
		case WIREFRAME:
			glNamedBufferData(
				buffer_vertices,
				sphere.size() * sizeof(float) * 3,
				sphere.data(),
				GL_DYNAMIC_DRAW);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			if (algorithm == Algorithm::DELAUNAY)
				glDrawArrays(GL_TRIANGLES, 0, sphere.size());
			else
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			// glDrawArrays(GL_TRIANGLES, 0, sphere.size());
			// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			break;
		case POINTS:
			glNamedBufferData(
				buffer_vertices,
				sphere_cloud.size() * sizeof(float) * 3,
				sphere_cloud.data(),
				GL_DYNAMIC_DRAW);

			glDrawArrays(GL_POINTS, 0, sphere_cloud.size());
			break;
		}

		glBindVertexArray(0);
		glUseProgram(0);

		if (ImGui::GetFrameCount() > 0) {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		SDL_GL_SwapWindow(window.sdl_window());
	}

	std::vector<vec3> sphere_points(size_t latitude = 14, size_t longitude = 14, float radius = 1.0f)
	{
		std::vector<vec3> points = std::vector<vec3>{ 3 * (latitude - 2) * longitude + 2 };

		uint32_t point_counter = 0;

		// Top point

		points[point_counter++] = vec3{ 0.0f, 0.0f, radius };

		// "Inner" points

		for (int v = 1; v < latitude-1; ++v) {
			for (int h = 0; h < longitude; ++h) {
				double angle_lat = double(v) / double(latitude-1) * glm::pi<double>() - glm::pi<double>() / 2.0;
				double angle_lon = double(h) / double(longitude) * glm::pi<double>() * 2.0;

				glm::mat4 transform = glm::mat4(1.0);
				transform = glm::rotate<float>(transform, angle_lon, vec3{ 0.0f, 0.0f, 1.0f });
				transform = glm::rotate<float>(transform, angle_lat, vec3{ 0.0f, 1.0f, 0.0f });

				glm::vec4 op = transform * glm::vec4{ radius, 0.0f, 0.0f, 1.0f };
				// vec3 p = vec3{ op } / op.w;
				vec3 p = vec3{ op };

				points[point_counter++] = p;
			}
		}

		// Bottom point

		points[point_counter++] = vec3{ 0.0f, 0.0f, -radius };

		return points;
	}

	std::vector<vec3> delaunay(const std::vector<vec3>& input)
	{
		using dvec3 = glm::dvec3;

		std::vector<dvec3> points;

		for (const auto& v : input)
			points.push_back(v);

		orgQhull::Qhull qhull;

		qhull.runQhull("delaunay", 3, points.size(), &points.data()->x, "Qt i");

		std::vector<vec3> output;

		normals.clear();
		for (const auto& f : qhull.facetList()) {
			for (const auto& v : f.vertices()) {
				const auto& coords = v.point().coordinates();
				const auto& vertex = vec3{ coords[0], coords[1], coords[2] };

				output.push_back(vertex);
				normals.push_back(glm::normalize(vertex));
			}
		}

		return output;
	}

	std::vector<vec3> marching_cubes(const std::vector<float>& input, const ivec3& field_size)
	{
		std::vector<float> new_input = input;

		MC::mcMesh mesh;
		MC::marching_cube(new_input.data(), field_size.x, field_size.y, field_size.z, mesh);

		std::vector<vec3> output;

		for (const auto& v : mesh.vertices) {
			auto transform = mat4{ 1.0f };
			transform = glm::scale(transform, vec3{ 3.0f/field_size.x, 3.0f/field_size.y, 3.0f/field_size.z });
			transform = glm::translate(transform, -vec3{ field_size  } * 0.63f);
			auto vector = transform * glm::vec4{ v.x, v.y, v.z, 1.0f };
			output.push_back(vec3{ vector });
		}

		indices.clear();
		for (const auto& i : mesh.indices)
			indices.push_back(i);

		normals.clear();
		for (const auto& n : mesh.normals)
			normals.push_back(-vec3{ n.x, n.y, n.z });

		return output;
	}

	std::vector<float> scalar_field(const std::vector<vec3>& input, const ivec3& field_size)
	{
		// find max and min x, y, z

		vec3 max = input[0];
		vec3 min = input[0];

		for (const auto& v : input) {
			if (v.x > max.x)
				max.x = v.x;
			if (v.y > max.y)
				max.y = v.y;
			if (v.z > max.z)
				max.z = v.z;

			if (v.x < min.x)
				min.x = v.x;
			if (v.y < min.y)
				min.y = v.y;
			if (v.z < min.z)
				min.z = v.z;
		}

		vec3 delta = glm::ceil(glm::abs(max) + glm::abs(min));

		std::vector<vec3> translated_input;

		for (const auto& v : input)
			translated_input.push_back(v + delta);

		max += delta;
		min += delta;

		vec3 scale = vec3{ 1.0f };
		scale.x = field_size.x / max.x;
		scale.y = field_size.y / max.y;
		scale.z = field_size.z / max.z;
		scale = glm::round(scale) - vec3{ 1.0f };

		std::vector<float> field;
		field.resize(field_size.x * field_size.y * field_size.z);

		for (auto& v : field)
			v = -1.0f;

		for (const auto& v : translated_input) {
			ivec3 index = glm::round(v * scale) - glm::vec3{ 1.0f };

			field[(index.z * field_size.y + index.y) * field_size.x + index.x] = 1.0f;

			/*
			ivec3 findex = glm::floor(v * scale) - glm::vec3{ 1.0f };
			ivec3 cindex = glm::ceil(v * scale) - glm::vec3{ 1.0f };

			if (cindex != findex) {
				field[(findex.z * field_size.y + findex.y) * field_size.x + findex.x] = 1.0f;
				field[(cindex.z * field_size.y + cindex.y) * field_size.x + cindex.x] = 1.0f;
			} else {
				field[(index.z * field_size.y + index.y) * field_size.x + index.x] = 1.0f;
			}*/
		}

		return field;
	}
};

int main(int argc, char** argv)
{
	Delaunay app;

	app.start();

	return 0;
}
