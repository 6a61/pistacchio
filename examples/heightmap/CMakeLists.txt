cmake_minimum_required(VERSION 3.22)
project(heightmap VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#===============================================================================
# Libraries
#===============================================================================

# Pistacchio

# add_library(pistacchio STATIC IMPORTED)
#
# set_target_properties(pistacchio PROPERTIES
# 	IMPORTED_LOCATION "D:/dev/pistacchio/lib/pistacchio.lib"
# 	INTERFACE_INCLUDE_DIRECTORIES "D:/dev/pistacchio/include")

# SDL

find_package(SDL2 CONFIG REQUIRED)

# GLM

find_package(GLM REQUIRED)

# glad

add_library(glad INTERFACE)
target_include_directories(glad INTERFACE thirdparty/glad/include)

# imgui

add_library(imgui STATIC)

target_sources(imgui PRIVATE
	thirdparty/imgui/imgui/imgui.cpp
	thirdparty/imgui/imgui/imgui_tables.cpp
	thirdparty/imgui/imgui/imgui_demo.cpp
	thirdparty/imgui/imgui/imgui_draw.cpp
	thirdparty/imgui/imgui/imgui_widgets.cpp
	thirdparty/imgui/imgui/imgui_impl_opengl3.cpp
	thirdparty/imgui/imgui/imgui_impl_sdl2.cpp)

target_include_directories(imgui PUBLIC thirdparty/imgui)
target_include_directories(imgui PUBLIC thirdparty/imgui/imgui)

target_link_libraries(imgui PRIVATE SDL2::SDL2)

# stb

add_library(stb INTERFACE)

target_include_directories(stb INTERFACE thirdparty/stb)

#===============================================================================
# Executable
#===============================================================================

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

target_sources(${PROJECT_NAME} PRIVATE
	src/heightmap.cc
	src/main.cc)

target_include_directories(${PROJECT_NAME} PRIVATE include src)

target_link_libraries(${PROJECT_NAME} PRIVATE
	glad
	imgui
	pistacchio
	stb
	SDL2::SDL2 $<${WIN32}: SDL2::SDL2main>
)
