add_executable(triangulation)

set_target_properties(triangulation PROPERTIES
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# third-party

if(true)
	target_sources(triangulation PRIVATE
		thirdparty/imgui/imgui.cpp
		thirdparty/imgui/imgui_tables.cpp
		thirdparty/imgui/imgui_demo.cpp
		thirdparty/imgui/imgui_draw.cpp
		thirdparty/imgui/imgui_widgets.cpp
		thirdparty/imgui/backends/imgui_impl_opengl3.cpp
		thirdparty/imgui/backends/imgui_impl_sdl2.cpp)

	target_include_directories(triangulation PUBLIC thirdparty/imgui/)
endif()

find_package(Qhull CONFIG REQUIRED)

# build

target_include_directories(triangulation PRIVATE include src)

target_sources(triangulation PRIVATE
	src/main.cc)

target_link_libraries(triangulation PRIVATE pistacchio Qhull::qhull_r Qhull::qhullcpp)
