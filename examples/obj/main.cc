#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/gl.h>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <SDL_video.h>
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl.h"
#include "imgui.h"
#include "pistacchio/app.hh"
#include "pistacchio/input.hh"
#include "pistacchio/log.hh"
#include "pistacchio/types.hh"
#include "pistacchio/filesystem/obj.hh"
#include "pistacchio/gl/shader.hh"
#include "pistacchio/gl/window.hh"

using vec3 = glm::vec3;

static auto _log = Log("OBJ");

class ObjApp : public App {
private:
	WindowGL window  = WindowGL("OBJ", Window::CENTERED, Window::CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
	ShaderGL shader = ShaderGL({
		{ ShaderGL::VERTEX, "default.vert" },
		{ ShaderGL::FRAGMENT, "default.frag" }
	});

	OBJ obj = OBJ::load("suzanne.obj");
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<u32> indices;

	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;

	vec3  camera_rotation    = { 0.0f, 0.0f, 0.0f };
	vec3  camera_position    = { 0.0f, 0.0f, -4.5f };
	vec3  object_color       = { 1.0f, 0.25f, 0.25f };
	vec3  model_rotation     = { 0.0f, 30.0f, 0.0f };
	vec3  light_position     = { 12.0f, 24.0f, 12.0f };
	vec3  light_color        = { 1.0f, 1.0f, 1.0f };
	float ambient_strength   = 0.2f;
	float specular_strength  = 0.5f;
	float model_opacity      = 1.0f;
	int   specular_shininess = 8;
	bool  wireframe          = false;
	bool  flat_shading       = false;

	u32 vao = GL_NONE;
	u32 buffer_vertices = GL_NONE;
	u32 buffer_normals = GL_NONE;
	u32 buffer_indices = GL_NONE;
public:
	ObjApp() : App(30, 60)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().IniFilename = nullptr;
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(window.sdl_window(), window.data());
		ImGui_ImplOpenGL3_Init("#version 450 core");

		for (const auto& v : obj.vertices)
			vertices.push_back(glm::vec3{ v.x, v.y, v.z });

		for (const auto& f : obj.faces)
			for (const auto& i : f.indices)
				indices.push_back(i.v - 1);

		normals.resize(vertices.size(), glm::vec3{ 0.0f, 0.0f, 0.0f });

		for (const auto& f : obj.faces) {
			for (const auto& i : f.indices) {
				const auto& v = vertices[i.v - 1];
				const auto& vn = obj.vertex_normals[i.vn - 1];

				glm::vec3& normal = normals[i.v - 1];
				normal += glm::vec3{ vn.x, vn.y, vn.z };
			}
		}

		for (auto& vn : normals) {
			vn = glm::normalize(vn);
		}

		projection = glm::perspective(glm::radians(45.0f), (float)window.width()/window.height(), 0.1f, 100.0f);
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glCreateBuffers(1, &buffer_vertices);
		glNamedBufferStorage(buffer_vertices,
			vertices.size() * sizeof(float) * 3,
			vertices.data(),
			GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &buffer_normals);
		glNamedBufferStorage(buffer_normals,
			normals.size() * sizeof(float) * 3,
			normals.data(),
			GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &buffer_indices);
		glNamedBufferStorage(buffer_indices,
			indices.size() * sizeof(uint32_t),
			indices.data(),
			GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &vao);

		glVertexArrayVertexBuffer(vao, 0, buffer_vertices, 0, sizeof(float) * 3);
		glVertexArrayVertexBuffer(vao, 1, buffer_normals, 0, sizeof(float) * 3);
		glVertexArrayElementBuffer(vao, buffer_indices);

		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 0, 0);

		glEnableVertexArrayAttrib(vao, 1);
		glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 1, 1);
	}

	void update(double dt) override
	{
		Input::update();

		for (const auto& event : Input::sdl()) {
			ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.type) {
			case SDL_QUIT:
				stop();
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				case SDL_WINDOWEVENT_RESIZED:
					float width = event.window.data1;
					float height = event.window.data2;

					glViewport(0, 0, width, height);
					projection = glm::perspective(glm::radians(45.0f), width/height, 0.1f, 100.0f);
				}
				break;
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(320, 0));

		if (ImGui::Begin("Settings##debug", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::PushItemWidth(-5.0f * ImGui::GetFontSize());

			ImGui::Text("Camera");
			ImGui::DragFloat3("Rotation##camrotX", &camera_rotation[0], 1.0f);
			ImGui::DragFloat3("Position##camposX", &camera_position[0], 0.05f);
			ImGui::Separator();

			ImGui::Text("Model");
			ImGui::DragFloat3("Rotation##modelRotation", &model_rotation[0], 1.0f);
			ImGui::ColorEdit3("Color##objectColor", &object_color.x);
			ImGui::SliderFloat("Opacity##model_opacity", &model_opacity, 0.0f, 1.0f);
			ImGui::Separator();

			ImGui::Text("Light");
			ImGui::DragFloat3("Position##lightPosition", &light_position[0], 0.05f);
			ImGui::ColorEdit3("Color##lightColor", &light_color.x);
			ImGui::SliderFloat("Ambient##ambientStrength", &ambient_strength, 0.0f, 1.0f);
			ImGui::SliderFloat("Specular##specularStrength", &specular_strength, 0.0f, 1.0f);
			ImGui::SliderInt("Shininess##specularShininess", &specular_shininess, 1, 256);
			ImGui::Separator();

			ImGui::Text("Misc.");
			ImGui::Checkbox("Wireframe", &wireframe);
			ImGui::SameLine();
			ImGui::Checkbox("Flat shading", &flat_shading);
		}; ImGui::End();

		ImGui::EndFrame();
	}

	void render(double alpha) override
	{
		float clear_color[4] = { 0.33f, 0.33f, 0.33f, 1.0f };
		float clear_depth = 1.0;
		glClearBufferfv(GL_COLOR, 0, clear_color);
		glClearBufferfv(GL_DEPTH, 0, &clear_depth);

		view = glm::mat4(1.0f);
		view = glm::translate(view, camera_position);
		view = glm::rotate(view, glm::radians(camera_rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(camera_rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(camera_rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(model_rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
		model = glm::rotate(model, glm::radians(model_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(model_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		shader.uniform("projection", projection);
		shader.uniform("model", model);
		shader.uniform("view", view);
		shader.uniform("light_position", light_position);
		shader.uniform("light_color", light_color);
		shader.uniform("object_color", object_color);
		shader.uniform("ambient_strength", ambient_strength);
		shader.uniform("flat_shading", flat_shading);
		shader.uniform("model_opacity", model_opacity);
		shader.uniform("view_position", camera_position);
		shader.uniform("specular_strength", specular_strength);
		shader.uniform("specular_shininess", specular_shininess);

		glUseProgram(shader.id());
		glBindVertexArray(vao);

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);

		if (ImGui::GetFrameCount() > 0) {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		SDL_GL_SwapWindow(window.sdl_window());
	}
};

int main(int argc, char** argv)
{
	ObjApp app;

	app.start();

	return 0;
}
