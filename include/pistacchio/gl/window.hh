#pragma once

#include "pistacchio/window.hh"

class WindowGL : public Window {
public:
	WindowGL(const std::string& title, int x, int y, int width, int height, uint32_t flags = 0);
	~WindowGL();

	void* data() override;
private:
	void* m_gl_context;
};
