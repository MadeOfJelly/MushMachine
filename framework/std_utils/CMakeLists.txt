cmake_minimum_required(VERSION 3.2)
project(std_utils CXX)

add_library(std_utils INTERFACE)

target_include_directories(std_utils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

##############################

# curr only scalar_range
add_library(std_utils_serialize INTERFACE)

target_include_directories(std_utils_serialize INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(std_utils_serialize INTERFACE
	nlohmann_json::nlohmann_json
)

##############################

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

