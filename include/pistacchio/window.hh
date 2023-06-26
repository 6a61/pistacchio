#pragma once

#include <cstdint>
#include <string>

#include <SDL2/SDL.h>

// Class that wraps SDL2 for creating a window.
class Window {
public:
	static const auto CENTERED = SDL_WINDOWPOS_CENTERED;
	static const auto UNDEFINED = SDL_WINDOWPOS_UNDEFINED;
	static const auto RESIZABLE = SDL_WINDOW_RESIZABLE;

	Window(const std::string& title, int x, int y, int width, int height, uint32_t flags);
	virtual ~Window();

	SDL_Window* sdl_window();
	virtual void* data();

	void resizable(bool flag);
	bool resizable();
	int width();
	int height();
	uint32_t id();
protected:
	SDL_Window* m_sdl_window;
};
