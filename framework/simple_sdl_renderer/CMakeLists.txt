cmake_minimum_required(VERSION 3.2)
project(simple_sdl_renderer_service CXX)

add_library(simple_sdl_renderer_service
	src/mm/simple_sdl_renderer/target.hpp

	src/mm/services/simple_sdl_renderer.hpp
	src/mm/services/simple_sdl_renderer.cpp
)

target_include_directories(simple_sdl_renderer_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(simple_sdl_renderer_service PUBLIC "${SDL2_INCLUDE_DIR}")

target_link_libraries(simple_sdl_renderer_service
	engine
	glm
	sdl_service
)

if(EMSCRIPTEN)
	set_target_properties(simple_sdl_renderer_service PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
	set_target_properties(simple_sdl_renderer_service PROPERTIES LINK_FLAGS "-s USE_SDL=2")
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

