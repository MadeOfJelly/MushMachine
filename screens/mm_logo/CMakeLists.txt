cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

project(mm_logo_screen CXX)

add_library(mm_logo_screen
	res/mush_machine_logo_1.svg.png.h

	src/mm/screens/mm_logo_screen.hpp
	src/mm/screens/mm_logo_screen.cpp
)

target_include_directories(mm_logo_screen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(mm_logo_screen
	entt
	glm
	engine
	screen_director
	organizer_scene

	opengl_renderer_s
	simple_sprite_render_task

	common_components
	transform_system

	random
)

#if (BUILD_TESTING)
	#add_subdirectory(test)
#endif()


