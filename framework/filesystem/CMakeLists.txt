cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

project(filesystem_service CXX)

add_library(filesystem_service
	src/mm/path_utils.hpp

	src/mm/services/filesystem.hpp
	src/mm/services/filesystem.cpp

	src/mm/fs_const_archiver.hpp
	src/mm/fs_const_archiver.cpp
)

target_include_directories(filesystem_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(filesystem_service PUBLIC
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
		target_compile_options(filesystem_service PUBLIC -sUSE_SDL=2)
		target_link_libraries(filesystem_service PUBLIC -sUSE_SDL=2)
	elseif(VCPKG_TARGET_TRIPLET)
		find_package(SDL2 CONFIG REQUIRED)

		target_link_libraries(filesystem_service
			PUBLIC
				SDL2::SDL2
				SDL2::SDL2main
				#SDL2::SDL2-static
		)
	else()
		#if not android or emscripten
		find_package(SDL2 REQUIRED)
		target_include_directories(filesystem_service PUBLIC "${SDL2_INCLUDE_DIR}")
		target_link_libraries(filesystem_service PUBLIC ${SDL2_LIBRARY})
		#endif
	endif()
endif()

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

