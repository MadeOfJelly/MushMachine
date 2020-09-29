cmake_minimum_required(VERSION 3.2)
project(sound_service CXX)

add_library(sound_service
	src/mm/services/sound_service.hpp
	src/mm/services/sound_service.cpp
)

target_include_directories(sound_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
#target_include_directories(service_sound PUBLIC "${SDL2_INCLUDE_DIR}")

target_link_libraries(sound_service
	#entt
	soloud
	logger
	engine
)

###############################

add_library(filesystem_soloud_file
	./src/mm/soloud_filesystem_file_impl.hpp
	./src/mm/soloud_filesystem_file_impl.cpp

	./src/mm/sound_loader_wav.hpp
	./src/mm/sound_loader_wav.cpp
)

target_link_libraries(filesystem_soloud_file
	filesystem_service
	soloud
)

###############################

add_library(soloud_json
	./src/mm/soloud_json.hpp
	./src/mm/soloud_json.cpp
)

target_include_directories(soloud_json PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(soloud_json
	soloud
	nlohmann_json::nlohmann_json
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

