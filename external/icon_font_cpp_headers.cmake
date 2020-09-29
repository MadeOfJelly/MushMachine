cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
add_library(icon_font_cpp_headers INTERFACE)
target_include_directories(icon_font_cpp_headers INTERFACE "${CMAKE_CURRENT_LIST_DIR}/IconFontCppHeaders/")

