cmake_minimum_required(VERSION 3.22)
project(pistacchio
	VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#===============================================================================
# Options
#===============================================================================

option(PISTACCHIO_ENABLE_FREETYPE "Enable Freetype" ON)
option(PISTACCHIO_ENABLE_OPENGL "Enable OpenGL" ON)
option(PISTACCHIO_ENABLE_VULKAN "Enable Vulkan" OFF)
option(PISTACCHIO_BUILD_EXAMPLES "Build examples" OFF)

#===============================================================================
# Status
#===============================================================================

message(STATUS "\n")
message(STATUS "")
message(STATUS "Pistacchio Framework, version ${PROJECT_VERSION}")
message(STATUS "")
message(STATUS "Enable Freetype: " ${PISTACCHIO_ENABLE_FREETYPE})
message(STATUS "Enable OpenGL: " ${PISTACCHIO_ENABLE_OPENGL})
message(STATUS "Enable Vulkan: " ${PISTACCHIO_ENABLE_VULKAN})
message(STATUS "Build examples: " ${PISTACCHIO_ENABLE_EXAMPLES})
message(STATUS "\n")

#===============================================================================
# Third party
#===============================================================================

# FreeType

if(PISTACCHIO_ENABLE_FREETYPE)
	find_package(Freetype REQUIRED)
endif()

# glad

if(PISTACCHIO_ENABLE_OPENGL)
	add_library(glad INTERFACE)
	target_include_directories(glad INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/include)
endif()

# GLM

find_package(glm REQUIRED)

# SDL

find_package(SDL2 CONFIG REQUIRED)

# stb

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb)

# Vulkan

if(PISTACCHIO_ENABLE_VULKAN)
	find_package(Vulkan REQUIRED)
endif()

#===============================================================================
# Build
#===============================================================================

add_library(pistacchio STATIC)

set_target_properties(pistacchio PROPERTIES
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_compile_definitions(pistacchio PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(pistacchio PRIVATE -Wall -Wextra)

target_link_libraries(pistacchio
	PUBLIC
		glm::glm
		stb
		$<${WIN32}: SDL2::SDL2main>
		SDL2::SDL2)

target_include_directories(pistacchio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(pistacchio PRIVATE
	src/app.cc
	src/log.cc
	src/input.cc
	src/time.cc
	src/window.cc
	src/filesystem/obj.cc)

if(PISTACCHIO_ENABLE_OPENGL)
	target_link_libraries(pistacchio PUBLIC glad)

	target_sources(pistacchio PRIVATE
		src/gl/shader.cc
		src/gl/texture.cc
		src/gl/window.cc)
endif()

if(PISTACCHIO_ENABLE_VULKAN)
	target_link_libraries(pistacchio
		PUBLIC Vulkan::Vulkan)

	target_sources(pistacchio PRIVATE
		src/vulkan/window.cc)
endif()

if(PISTACCHIO_ENABLE_FREETYPE)
	target_link_libraries(pistacchio PUBLIC Freetype::Freetype)

	target_sources(pistacchio PRIVATE src/font.cc)
endif()

#===============================================================================
# Examples
#===============================================================================

if(PISTACCHIO_BUILD_EXAMPLES)
	add_subdirectory(examples/imgui)
	add_subdirectory(examples/obj)
	add_subdirectory(examples/red-rom-viewer)
	add_subdirectory(examples/triangulation)
endif()
