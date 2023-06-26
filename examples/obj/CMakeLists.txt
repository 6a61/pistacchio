add_executable(obj)

set_target_properties(obj PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF)

target_sources(obj PRIVATE
	main.cc
	imgui/imgui.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_opengl3.cpp
	imgui/backends/imgui_impl_sdl.cpp)

target_include_directories(obj PRIVATE imgui/)

target_link_libraries(obj PRIVATE pistacchio)
