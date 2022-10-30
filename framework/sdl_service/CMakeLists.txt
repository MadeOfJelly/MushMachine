cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(sdl_service CXX)

add_library(sdl_service
	src/mm/logo-f6c-square.png.h

	src/mm/services/sdl_service.hpp
	src/mm/services/sdl_service.cpp
)

target_include_directories(sdl_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(sdl_service PUBLIC "${SDL2_INCLUDE_DIR}")

target_link_libraries(sdl_service PUBLIC
	entt
	logger
	#glm
	engine
)


#if android
#target_link_libraries(sdl_service SDL)
#endif

if(EMSCRIPTEN)
	target_compile_options(sdl_service PUBLIC -sUSE_SDL=2)
	target_link_libraries(sdl_service PUBLIC -sUSE_SDL=2)
else()
	#if not android or emscripten
	target_include_directories(sdl_service PUBLIC "${SDL2_INCLUDE_DIR}")
	target_link_libraries(sdl_service PUBLIC ${SDL2_LIBRARY})
	#endif
endif()

if(MM_OPENGL_3_GLES)
	target_link_libraries(sdl_service PRIVATE "GL") # TODO: make more specific
else()
	target_link_libraries(sdl_service PRIVATE glad)
endif()

if(VCPKG_TARGET_TRIPLET)
	target_link_libraries(sdl_service
		PUBLIC
			SDL2::SDL2
		PRIVATE
			SDL2::SDL2main
			SDL2::SDL2-static
	)
endif()

if(BUILD_TESTING)
	add_subdirectory(test)
endif()

