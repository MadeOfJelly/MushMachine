cmake_minimum_required(VERSION 3.2)
project(transfrom_system CXX)

add_library(transform_system
	src/mm/systems/transform.hpp
	src/mm/systems/transform.cpp
)

target_include_directories(transform_system PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(transform_system
	entt
	glm
	engine
	common_components
	tracy_client
)

#if (BUILD_TESTING)
	#add_subdirectory(test)
#endif()


