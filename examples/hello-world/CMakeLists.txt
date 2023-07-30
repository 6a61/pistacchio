cmake_minimum_required(VERSION 3.22)
project(hello-world VERSION 0.1.0)

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

find_package(glm REQUIRED)

# glad

add_library(${PROJECT_NAME}_glad INTERFACE)
target_include_directories(${PROJECT_NAME}_glad INTERFACE thirdparty/glad/include)

# imgui

add_library(${PROJECT_NAME}_imgui STATIC)

target_sources(${PROJECT_NAME}_imgui PRIVATE
	thirdparty/imgui/imgui/imgui.cpp
	thirdparty/imgui/imgui/imgui_tables.cpp
	thirdparty/imgui/imgui/imgui_demo.cpp
	thirdparty/imgui/imgui/imgui_draw.cpp
	thirdparty/imgui/imgui/imgui_widgets.cpp
	thirdparty/imgui/imgui/imgui_impl_opengl3.cpp
	thirdparty/imgui/imgui/imgui_impl_sdl2.cpp)

target_include_directories(${PROJECT_NAME}_imgui PUBLIC thirdparty/imgui)
target_include_directories(${PROJECT_NAME}_imgui PUBLIC thirdparty/imgui/imgui)

target_link_libraries(${PROJECT_NAME}_imgui PRIVATE SDL2::SDL2)

#===============================================================================
# Executable
#===============================================================================

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

target_sources(${PROJECT_NAME} PRIVATE
	src/main.cc)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
	pistacchio
	SDL2::SDL2 $<${WIN32}: SDL2::SDL2main>
	${PROJECT_NAME}_glad
	${PROJECT_NAME}_imgui)
