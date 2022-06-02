cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(vulkan_renderer CXX)

add_library(vulkan_renderer
	src/mm/services/vulkan_renderer.hpp
	src/mm/services/vulkan_renderer.cpp
)

target_include_directories(vulkan_renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(vulkan_renderer
	Vulkan::Headers

	engine
	logger

	sdl_service
)

########################

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

