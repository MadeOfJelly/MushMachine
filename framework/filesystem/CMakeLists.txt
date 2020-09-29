cmake_minimum_required(VERSION 3.2)
project(filesystem_service CXX)

add_library(filesystem_service
	src/mm/path_utils.hpp

	src/mm/services/filesystem.hpp
	src/mm/services/filesystem.cpp

	src/mm/fs_const_archiver.hpp
	src/mm/fs_const_archiver.cpp
)

target_include_directories(filesystem_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(filesystem_service
	engine
	logger
	entt
	nlohmann_json::nlohmann_json
	physfs-static # TODO: fix this
	std_utils
)

if(NOT MM_HEADLESS)

	#if android
	#target_link_libraries(filesystem_service SDL)
	#endif

	if(EMSCRIPTEN)
		set_target_properties(filesystem_service PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
		set_target_properties(filesystem_service PROPERTIES LINK_FLAGS "-s USE_SDL=2")
	else()
		#if not android or emscripten
		target_include_directories(filesystem_service PUBLIC "${SDL2_INCLUDE_DIR}")
		target_link_libraries(filesystem_service ${SDL2_LIBRARY})
		#endif
	endif()
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

