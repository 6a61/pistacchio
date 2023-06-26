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
#include "pistacchio/filesystem/obj.hh"
#include "pistacchio/gl/shader.hh"
#include "pistacchio/gl/window.hh"

static auto _log = Log("OBJ");

class ObjApp : public App {
private:
	WindowGL window  = WindowGL("OBJ", Window::CENTERED, Window::CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
	ShaderGL shader = ShaderGL({
		{ ShaderGL::VERTEX, "default.vert" },
		{ ShaderGL::FRAGMENT, "default.frag" }
	});
	OBJ obj = OBJ::load("suzanne.obj");
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<uint32_t> indices;

	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;

	float camera_rotation[3] = { 0.0f, 0.0f, 0.0f };
	float camera_position[3] = { 0.0f, 0.0f, -4.5f };
	glm::vec3 object_color = glm::vec3{ 1.0f };
	glm::vec3 model_rotation = glm::vec3{ 0.0f, 30.0f, 0.0f };
	glm::vec3 light_position = glm::vec3{ 0.0f, 3.0f, 3.0f };
	glm::vec3 light_color = glm::vec3{ 1.0f };
	float ambient_strength = 0.2f;
	float model_opacity = 1.0f;
	bool wireframe = false;
	bool flat_shading = true;

	uint32_t vao = GL_NONE;
	uint32_t buffer_vertices = GL_NONE;
	uint32_t buffer_normals = GL_NONE;
	uint32_t buffer_indices = GL_NONE;
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

		ImGui::Begin("##debug", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PushItemWidth(-5.0f * ImGui::GetFontSize());

		ImGui::Text("Camera");
		ImGui::DragFloat3("Rotation##camrotX", &camera_rotation[0], 1.0f, -360.0f, 360.0f);
		ImGui::DragFloat3("Position##camposX", &camera_position[0], 0.05f, -10.0f, 10.0f);
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Model");
		ImGui::DragFloat3("Rotation##modelRotation", &model_rotation[0], 1.0f, -360.0f, 360.0f);
		ImGui::ColorEdit3("Color##objectColor", &object_color.x);
		ImGui::SliderFloat("Opacity##model_opacity", &model_opacity, 0.0f, 1.0f);
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Light");
		ImGui::DragFloat3("Position##lightPosition", &light_position[0], 0.05f, -100.0f, 100.0f);
		ImGui::ColorEdit3("Color##lightColor", &light_color.x);
		ImGui::SliderFloat("Ambient##ambientStrength", &ambient_strength, 0.0f, 1.0f);
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Misc.");
		ImGui::Checkbox("Wireframe", &wireframe);
		ImGui::SameLine();
		ImGui::Checkbox("Flat shading", &flat_shading);

		// ImGui::ShowDemoWindow();

		ImGui::End();

		ImGui::EndFrame();
	}

	void render(double alpha) override
	{
		float clear_color[4] = { 0.33f, 0.33f, 0.33f, 1.0f };
		float clear_depth = 1.0;
		glClearBufferfv(GL_COLOR, 0, clear_color);
		glClearBufferfv(GL_DEPTH, 0, &clear_depth);

		view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3{camera_position[0], camera_position[1], camera_position[2]});
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
