cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(simple_velocity_system CXX)

add_library(simple_velocity_system
	src/mm/systems/simple_velocity_system2d.hpp
	src/mm/systems/simple_velocity_system2d.cpp
)

target_include_directories(simple_velocity_system PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(simple_velocity_system
	entt
	glm
	engine
	common_components
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


