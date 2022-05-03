cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(systems)

add_subdirectory(transform)
add_subdirectory(simple_velocity)

if(NOT MM_HEADLESS)
	add_subdirectory(player_velocity)
	add_subdirectory(fast_sky_sun)
endif()

