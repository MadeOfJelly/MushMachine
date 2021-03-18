cmake_minimum_required(VERSION 3.2)
project(std_utils CXX)

add_library(std_utils INTERFACE)

target_include_directories(std_utils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


