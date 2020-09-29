cmake_minimum_required(VERSION 3.2)
project(opengl_primitives CXX)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE HPP_FILES src/*.hpp)

add_library(opengl_primitives ${CPP_FILES} ${HPP_FILES})

target_include_directories(opengl_primitives PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(opengl_primitives
	glm
	stb_image

	logger
	resource_manager
	filesystem_service
)

if(MM_OPENGL_3_GLES)
	target_link_libraries(opengl_primitives "GL") # TODO: make more specific
	if(EMSCRIPTEN)
		set_target_properties(opengl_primitives PROPERTIES COMPILE_FLAGS "-s USE_WEBGL2=1")
		set_target_properties(opengl_primitives PROPERTIES LINK_FLAGS "-s USE_WEBGL2=1")
	endif()

else()
	target_link_libraries(opengl_primitives glad)
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


