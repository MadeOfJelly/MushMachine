cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(stb CXX)

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(stb_image "stb/stb_image.h" "stb_image.cpp")
target_link_libraries(stb_image stb)

add_library(stb_image_write "stb/stb_image_write.h" "stb_image_write.cpp")
target_link_libraries(stb_image_write stb)

add_library(stb_perlin "stb/stb_perlin.h" "stb_perlin.cpp")
target_link_libraries(stb_perlin stb)

add_library(stb_rect_pack "stb/stb_rect_pack.h" "stb_rect_pack.cpp")
target_link_libraries(stb_rect_pack stb)

