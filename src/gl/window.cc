#include <iostream>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <SDL.h>

#include "pistacchio/window.hh"
#include "pistacchio/gl/window.hh"
#include "pistacchio/log.hh"

static auto _log = Log("Window GL");

WindowGL::WindowGL(const std::string& title, int x, int y, int width, int height, uint32_t flags) :
	Window(title, x, y, width, height, flags | SDL_WINDOW_OPENGL)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	m_gl_context = SDL_GL_CreateContext(m_sdl_window);

	if (!m_gl_context)
		_log.error("Unable to create OpenGL context");

	if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)))
		_log.error("Unable to load OpenGL functions");

	glViewport(0, 0, width, height);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

WindowGL::~WindowGL()
{
	if (m_gl_context)
		SDL_GL_DeleteContext(m_gl_context);

	m_gl_context = nullptr;
}

void* WindowGL::data()
{
	return m_gl_context;
}
