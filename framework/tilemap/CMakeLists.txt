cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(tilemap CXX)

add_library(tilemap
	./src/mm/tilemap.hpp
	./src/mm/tilemap.cpp
)

target_include_directories(tilemap PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(tilemap
	filesystem_service
	tilemap_render_task
)

##########################

# TODO: test!!
#if (BUILD_TESTING)
	#add_subdirectory(test)
#endif()


