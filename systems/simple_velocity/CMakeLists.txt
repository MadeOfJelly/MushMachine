cmake_minimum_required(VERSION 3.2)
project(simple_velocity_system CXX)

set(HPP_FILES
	src/mm/systems/simple_velocity_system2d.hpp
)

set(CPP_FILES
)

add_library(simple_velocity_system ${CPP_FILES} ${HPP_FILES})

target_include_directories(simple_velocity_system PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(simple_velocity_system
	entt
	glm
	engine
	common_components
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


