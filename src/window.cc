#include <SDL.h>
#include <SDL_video.h>

#include "pistacchio/log.hh"
#include "pistacchio/window.hh"

static auto _log = Log("Window");

Window::Window(const std::string& title, int x, int y, int width, int height, uint32_t flags) :
	m_sdl_window(SDL_CreateWindow(title.c_str(), x, y, width, height, flags))
{
	if (!m_sdl_window)
		_log.error("Unable to create SDL window");
}

Window::~Window()
{
	if (m_sdl_window)
		SDL_DestroyWindow(m_sdl_window);

	m_sdl_window = nullptr;
}

SDL_Window* Window::sdl_window()
{
	return m_sdl_window;
}

void* Window::data()
{
	return nullptr;
}

void Window::resizable(bool flag)
{
	SDL_SetWindowResizable(m_sdl_window, (SDL_bool)flag);
}

bool Window::resizable()
{
	uint32_t flags = SDL_GetWindowFlags(m_sdl_window);

	return flags & SDL_WINDOW_RESIZABLE;
}

int Window::width()
{
	int width = 0;
	SDL_GetWindowSize(m_sdl_window, &width, nullptr);
	return width;
}

int Window::height()
{
	int height = 0;
	SDL_GetWindowSize(m_sdl_window, nullptr, &height);
	return height;
}

uint32_t Window::id()
{
	return SDL_GetWindowID(m_sdl_window);
}
