cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

project(input_service CXX)

add_library(input_service
	src/mm/services/input_service.hpp
	src/mm/services/input_service.cpp
)

target_include_directories(input_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(input_service
	engine
	glm
	sdl_service
)

#if(NOT EMSCRIPTEN)
	#target_include_directories(input_service PUBLIC "${SDL2_INCLUDE_DIR}")
#endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

