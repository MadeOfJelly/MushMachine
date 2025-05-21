cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

project(screen_director CXX)

add_library(screen_director
	src/mm/services/screen_director.hpp
	src/mm/services/screen_director.cpp
)

target_include_directories(screen_director PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(screen_director
	engine
	logger
	entt
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

