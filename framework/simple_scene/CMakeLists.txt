cmake_minimum_required(VERSION 3.2)
project(simple_scene CXX)

add_library(simple_scene
	src/mm/services/simple_scene.hpp
	src/mm/services/simple_scene.cpp
)

target_include_directories(simple_scene PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(simple_scene
	engine
	common_components
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

