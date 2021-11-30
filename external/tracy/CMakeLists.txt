cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

add_library(tracy_client
	"${CMAKE_CURRENT_LIST_DIR}/tracy/Tracy.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/tracy/TracyLua.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/tracy/TracyVulkan.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/tracy/TracyOpenGL.hpp"

	"${CMAKE_CURRENT_LIST_DIR}/tracy/TracyClient.cpp"
)

target_compile_features(tracy_client PUBLIC cxx_std_17)

if(TRACY_ENABLE)
	target_compile_definitions(tracy_client PUBLIC TRACY_ENABLE)
	#target_compile_definitions(tracy_client PUBLIC TRACY_NO_SYSTEM_TRACING)
	message("Enabled TRACY")
endif()

target_include_directories(tracy_client PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

if(UNIX)
	target_link_libraries(tracy_client dl)
endif()

if(WIN32)
	target_link_libraries(tracy_client ws2_32 dbghelp)
endif()

