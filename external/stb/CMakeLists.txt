cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(stb CXX)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(stb_image "stb/stb_image.h" "stb_image.cpp")
target_link_libraries(stb_image stb)

add_library(stb_perlin "stb/stb_perlin.h" "stb_perlin.cpp")
target_link_libraries(stb_perlin stb)

