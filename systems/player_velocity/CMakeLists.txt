cmake_minimum_required(VERSION 3.2)
project(player_velocity_system CXX)

add_library(player_velocity_system
	src/mm/systems/player_velocity2d_system.hpp
	src/mm/systems/player_velocity2d_system.cpp
)

target_include_directories(player_velocity_system PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(player_velocity_system
	entt
	glm
	engine
	common_components
	input_service
)

if(EMSCRIPTEN)
	set_target_properties(player_velocity_system PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
	set_target_properties(player_velocity_system PROPERTIES LINK_FLAGS "-s USE_SDL=2")
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


