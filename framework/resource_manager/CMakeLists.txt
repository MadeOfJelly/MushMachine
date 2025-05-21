cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

project(resource_manager CXX)

add_library(resource_manager INTERFACE)

target_include_directories(resource_manager INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(resource_manager INTERFACE
	entt
	logger
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


