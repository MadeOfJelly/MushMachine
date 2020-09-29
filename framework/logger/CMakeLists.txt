cmake_minimum_required(VERSION 3.2)
project(logger CXX)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE HPP_FILES src/*.hpp)

add_library(logger ${CPP_FILES} ${HPP_FILES})

target_include_directories(logger PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

#target_compile_definitions(logger PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_compile_definitions(logger PUBLIC SPDLOG_ACTIVE_LEVEL=0)

target_link_libraries(logger
	spdlog
	tracy_client
)

#if (BUILD_TESTING)
	#add_subdirectory(test)
#endif()


