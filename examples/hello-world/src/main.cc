#include <glad/gl.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <pistacchio/gl/window.hh>
#include <pistacchio/app.hh>
#include <pistacchio/input.hh>

class HelloWorld : public App {
public:
	WindowGL window = WindowGL{ "Hello, world", Window::CENTERED, Window::CENTERED, 1280, 720 };

	HelloWorld() : App(60.0, 120.0)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(window.sdl_window(), window.data());
		ImGui_ImplOpenGL3_Init("#version 450 core");
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

		ImGui::ShowDemoWindow();

		ImGui::EndFrame();
	}

	void render(double alpha) override
	{
		float clear_color[4] = { 0.33f, 0.33f, 0.33f, 0.33f };
		glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);

		if (ImGui::GetFrameCount() > 0) {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		SDL_GL_SwapWindow(window.sdl_window());
	}
};

int main(int argc, char** argv)
{
	HelloWorld app;
	app.start();

	return 0;
}
