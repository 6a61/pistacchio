#pragma once

#include <vector>
#include <SDL2/SDL_events.h>

class Input {
private:
	static std::vector<SDL_Event> s_events;

	Input() = default;
public:
	static void update();
	static std::vector<SDL_Event> sdl();
};
