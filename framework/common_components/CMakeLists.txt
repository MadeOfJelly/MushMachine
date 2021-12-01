cmake_minimum_required(VERSION 3.2)
project(common_components CXX)

add_library(common_components INTERFACE)

target_include_directories(common_components INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(common_components INTERFACE
	entt
	glm
)

##########################

add_library(common_components_serialize_json INTERFACE)

target_include_directories(common_components_serialize_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(common_components_serialize_json INTERFACE
	common_components
	nlohmann_json::nlohmann_json
)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()

