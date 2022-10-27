cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

if(NOT EMSCRIPTEN)
	add_library(tracy_client
		"${CMAKE_CURRENT_LIST_DIR}/tracy/public/tracy/Tracy.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/tracy/public/tracy/TracyLua.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/tracy/public/tracy/TracyVulkan.hpp"
		"${CMAKE_CURRENT_LIST_DIR}/tracy/public/tracy/TracyOpenGL.hpp"

		"${CMAKE_CURRENT_LIST_DIR}/tracy/public/TracyClient.cpp"
	)

	if(TRACY_ENABLE)
		target_compile_definitions(tracy_client PUBLIC TRACY_ENABLE)
		#target_compile_definitions(tracy_client PUBLIC TRACY_NO_SYSTEM_TRACING)
		message("Enabled TRACY")
	endif()

	target_compile_features(tracy_client PUBLIC cxx_std_17)

	target_include_directories(tracy_client PUBLIC "${CMAKE_CURRENT_LIST_DIR}/tracy/public")

	if(UNIX)
		target_link_libraries(tracy_client dl)
	endif()

	if(WIN32)
		target_link_libraries(tracy_client ws2_32 dbghelp)
	endif()
else() # EMSCRIPTEN
	add_library(tracy_client INTERFACE)

	target_compile_features(tracy_client INTERFACE cxx_std_17)

	target_include_directories(tracy_client INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
endif()


