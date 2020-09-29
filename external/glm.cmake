cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${CMAKE_CURRENT_LIST_DIR}/glm")

