cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE "${CMAKE_CURRENT_LIST_DIR}/entt/src")
target_compile_features(entt INTERFACE cxx_std_17)

