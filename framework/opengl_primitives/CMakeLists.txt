cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(opengl_primitives CXX)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE HPP_FILES src/*.hpp)

add_library(opengl_primitives
	src/mm/opengl/buffer.hpp
	src/mm/opengl/buffer.cpp
	src/mm/opengl/fbo_builder.hpp
	src/mm/opengl/fbo_builder.cpp
	src/mm/opengl/frame_buffer_object.hpp
	src/mm/opengl/frame_buffer_object.cpp
	src/mm/opengl/instance_buffer.hpp
	src/mm/opengl/shader.hpp
	src/mm/opengl/shader.cpp
	src/mm/opengl/shader_builder.hpp
	src/mm/opengl/shader_builder.cpp
	src/mm/opengl/spritesheet.hpp
	src/mm/opengl/texture.hpp
	src/mm/opengl/texture.cpp
	src/mm/opengl/texture_loader.hpp
	src/mm/opengl/texture_loader.cpp
	src/mm/opengl/vertex_array_object.hpp
	src/mm/opengl/vertex_array_object.cpp


	src/mm/opengl/components/texture.hpp
)

target_include_directories(opengl_primitives PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(opengl_primitives
	glm
	stb_image
	#sdl
	sdl_service

	logger
	resource_manager
	filesystem_service
)

if(MM_OPENGL_3_GLES)
	target_link_libraries(opengl_primitives "GL") # TODO: make more specific
	if(EMSCRIPTEN)
		set_target_properties(opengl_primitives PROPERTIES COMPILE_FLAGS "-s USE_WEBGL2=1 -s USE_SDL=2")
		set_target_properties(opengl_primitives PROPERTIES LINK_FLAGS "-s USE_WEBGL2=1 -s USE_SDL=2")
	endif()

else()
	target_link_libraries(opengl_primitives glad)
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


