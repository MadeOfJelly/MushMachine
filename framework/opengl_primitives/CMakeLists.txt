cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(opengl_primitives CXX)

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

target_link_libraries(opengl_primitives PUBLIC
	glm
	stb_image
	#sdl
	sdl_service

	logger
	resource_manager
	filesystem_service
)

if(MM_OPENGL_3_GLES)
	target_link_libraries(opengl_primitives PUBLIC "GL") # TODO: make more specific
	if(EMSCRIPTEN)
		# USE_SDL=2 inherited from sdl_service
		# TODO: USE_WEBGL2 -> MIN_WEBGL_VERSION=2 maybe
		# technically only FULL_ES3 would be needed
		target_link_libraries(opengl_primitives PUBLIC -sUSE_WEBGL2=1 -sFULL_ES3=1)
	endif()

else()
	target_link_libraries(opengl_primitives PUBLIC glad)
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


