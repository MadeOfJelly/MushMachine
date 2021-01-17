cmake_minimum_required(VERSION 3.2)
project(organizer_scene CXX)

add_library(organizer_scene
	src/mm/services/organizer_scene.hpp
	src/mm/services/organizer_scene.cpp
)

target_include_directories(organizer_scene PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(organizer_scene
	engine
	common_components
)

if (BUILD_TESTING)
	#add_subdirectory(test)
endif()

