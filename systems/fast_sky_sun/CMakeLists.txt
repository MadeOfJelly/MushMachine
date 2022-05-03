cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(fast_sky_sun_system CXX)

add_library(fast_sky_sun_system
	src/mm/systems/fast_sky_sun_system.hpp
	src/mm/systems/fast_sky_sun_system.cpp
)

target_include_directories(fast_sky_sun_system PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(fast_sky_sun_system
	engine
	fast_sky_render_task
	common_components
)

if(EMSCRIPTEN)
	set_target_properties(fast_sky_sun_system PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
	set_target_properties(fast_sky_sun_system PROPERTIES LINK_FLAGS "-s USE_SDL=2")
endif()

