cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(s6zer CXX)

add_library(s6zer INTERFACE
	#./src/s6zer/stream.hpp
	#./src/s6zer/serialize.hpp
)

add_library(MM::s6zer ALIAS s6zer)

target_include_directories(s6zer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_features(s6zer INTERFACE cxx_std_17)

#target_link_libraries(s6zer
	#INTERFACE
#)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

