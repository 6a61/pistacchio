#define STB_IMAGE_IMPLEMENTATION

#include <glad/gl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <pistacchio/app.hh>
#include <pistacchio/input.hh>
#include <pistacchio/log.hh>
#include <pistacchio/types.hh>
#include <pistacchio/filesystem/obj.hh>
#include <pistacchio/gl/shader.hh>
#include <pistacchio/gl/window.hh>
#include <pistacchio/math/vector3.hh>
#include "heightmap.hh"

using vec3 = glm::vec3;

auto _log = Log("Main");

class HeightmapApp : public App {
public:
	WindowGL window = WindowGL( "Heightmap", Window::CENTERED, Window::CENTERED, 1280, 720 );
	ShaderGL shader = ShaderGL({
		{ ShaderGL::VERTEX,   "data/shaders/default.vert" },
		{ ShaderGL::FRAGMENT, "data/shaders/default.frag" },
	});

	Heightmap heightmap = Heightmap::load("data/heightmap.png");
	Heightmap::Mesh model_heightmap = heightmap.mesh();

	glm::vec3 heightmap_scale = glm::vec3{ 0.25f, 4.0f, 0.25f };
	glm::vec3 camera_rotation = { 20.0f, 33.0f, 0.0f };
	glm::vec3 camera_position = { 0.0f, 0.0f, -20.0f };
	glm::vec3 object_color = glm::vec3{ 1.0f };
	glm::vec3 model_rotation = glm::vec3{ 0.0f, 30.0f, 0.0f };
	glm::vec3 light_position = glm::vec3{ heightmap.width / 2.0f, 100.0f, heightmap.height / 2.0f };
	glm::vec3 light_color = glm::vec3{ 1.0f };
	float ambient_strength = 0.1f;
	float model_opacity = 1.0f;
	bool wireframe = false;
	bool flat_shading = false;

	u32 vao = GL_NONE;
	u32 buffer_vertices = GL_NONE;
	u32 buffer_normals = GL_NONE;
	u32 buffer_indices = GL_NONE;

	int upload_heightmap(const Heightmap::Mesh& model)
	{
		glNamedBufferData(buffer_vertices,
			model.vertices.size() * sizeof(float) * 3,
			model.vertices.data(),
			GL_DYNAMIC_DRAW);

		glNamedBufferData(buffer_indices,
			model.indices.size() * sizeof(u32),
			model.indices.data(),
			GL_DYNAMIC_DRAW);

		glNamedBufferData(buffer_normals,
			model.normals.size() * sizeof(float) * 3,
			model.normals.data(),
			GL_DYNAMIC_DRAW);

		return model.indices.size();
	}

	HeightmapApp() : App(60.0, 120.0)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(window.sdl_window(), window.data());
		ImGui_ImplOpenGL3_Init("#version 450 core");

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glCreateBuffers(1, &buffer_vertices);
		glCreateBuffers(1, &buffer_normals);
		glCreateBuffers(1, &buffer_indices);

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
			case SDL_QUIT: {
				this->stop();
			}
			break;
			}
		}

		ImGui_ImplSDL2_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		// ImGui::ShowDemoWindow();

		if (ImGui::Begin("Settings##debug", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::PushItemWidth(-5.0f * ImGui::GetFontSize());

			ImGui::Text("Camera");
			ImGui::DragFloat3("Rotation##camrotX", &camera_rotation[0]);
			ImGui::DragFloat3("Position##camposX", &camera_position[0]);
			ImGui::Separator();

			ImGui::Text("Light");
			ImGui::DragFloat3("Position##lightPosition", &light_position[0]);
			ImGui::ColorEdit3("Color##lightColor", &light_color.x);
			ImGui::SliderFloat("Ambient##ambientStrength", &ambient_strength, 0.0f, 1.0f);
			ImGui::Separator();

			ImGui::Text("Heightmap");
			ImGui::DragFloat3("Scale##heightmapScale", &heightmap_scale[0]);
			ImGui::Separator();

			ImGui::Text("Misc.");
			ImGui::Checkbox("Wireframe", &wireframe);
			ImGui::SameLine();
			ImGui::Checkbox("Flat shading", &flat_shading);
		}
		ImGui::End();

		ImGui::EndFrame();
	}

	void render(double alpha) override
	{
		float clear_color[4] = { 0.33f, 0.33f, 0.33f, 1.0f };
		glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);

		float clear_depth = 1.0f;
		glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clear_depth);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window.width()/window.height(), 0.1f, 1000.0f);
		glm::mat4 model = glm::mat4(1.0f);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, camera_position);
		view = glm::rotate(view, glm::radians(camera_rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
		view = glm::rotate(view, glm::radians(camera_rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
		view = glm::rotate(view, glm::radians(camera_rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });

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

		model = glm::mat4(1.0f);
		model = glm::scale(model, heightmap_scale);
		model = glm::translate(model, glm::vec3{ -heightmap.width / 2, 0, -heightmap.height / 2  });
		shader.uniform("model", model);
		glDrawElements(GL_TRIANGLES, upload_heightmap(model_heightmap), GL_UNSIGNED_INT, 0);

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
	HeightmapApp app;
	app.start();

	return 0;
}
