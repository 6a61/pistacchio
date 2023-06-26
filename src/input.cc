#include "pistacchio/input.hh"

std::vector<SDL_Event> Input::s_events;

void Input::update()
{
	s_events.clear();

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		s_events.push_back(event);
	}
}

std::vector<SDL_Event> Input::sdl()
{
	return s_events;
}
