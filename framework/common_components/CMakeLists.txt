cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(common_components CXX)

add_library(common_components INTERFACE)

add_library(MM::common_components ALIAS common_components)

target_include_directories(common_components INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(common_components INTERFACE
	entt
	glm
)

##########################

add_library(common_components_serialize_s6zer INTERFACE)

add_library(MM::common_components_serialize_s6zer ALIAS common_components_serialize_s6zer)

target_include_directories(common_components_serialize_s6zer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(common_components_serialize_s6zer INTERFACE
	common_components
	s6zer
)

##########################

add_library(common_components_serialize_json INTERFACE)

add_library(MM::common_components_serialize_json ALIAS common_components_serialize_json)

target_include_directories(common_components_serialize_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(common_components_serialize_json INTERFACE
	common_components
	nlohmann_json::nlohmann_json
)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()

