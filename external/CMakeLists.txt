cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

# external libs

if (BUILD_TESTING)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	add_subdirectory("googletest")
endif()

add_subdirectory("spdlog")

#include("${CMAKE_CURRENT_SOURCE_DIR}/tracy.cmake")
add_subdirectory("tracy")

include("${CMAKE_CURRENT_SOURCE_DIR}/entt.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/glm.cmake")

add_subdirectory("SquirrelNoise")

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")
add_subdirectory("json") # link with "nlohmann_json::nlohmann_json"

add_subdirectory("physfs")

if(NOT MM_HEADLESS)
	if(NOT EMSCRIPTEN)
		# TODO: move into imgui and soload, since they are the one requireing this
		if(VCPKG_TARGET_TRIPLET)
			find_package(SDL2 CONFIG REQUIRED)
		else() # HACK: fix sdl find module
			find_package(SDL2 REQUIRED)
		endif()
	endif()

	if(NOT MM_OPENGL_3_GLES)
		# opengl3 loader
		add_subdirectory("glad-debug")
	endif()

	# stb utilies
	add_subdirectory("stb")

	# nice symbol font integration
	include("${CMAKE_CURRENT_SOURCE_DIR}/icon_font_cpp_headers.cmake")

	# (debug) gui
	add_subdirectory("imgui")

	# sound, uses sdl2-static backend
	add_subdirectory("soloud")
endif()


