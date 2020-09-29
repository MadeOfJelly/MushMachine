cmake_minimum_required(VERSION 3.2)
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

if(EMSCRIPTEN)
	set_target_properties(input_service PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
	set_target_properties(input_service PROPERTIES LINK_FLAGS "-s USE_SDL=2")
else()
	target_include_directories(input_service PUBLIC "${SDL2_INCLUDE_DIR}")
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

