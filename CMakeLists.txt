cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# cmake setup begin
project(MushMachine C CXX)

# defaulting to debug mode, if not specified
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

# setup my vim ycm :D
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# paths
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# more paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# add this to your projects cmake to enable ipo
#include(CheckIPOSupported)
#check_ipo_supported()
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# enable test
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
	#enable_testing()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/mm_options_and_defines.cmake")

# external libs
add_subdirectory("external") # before increasing warn levels (sad :( )

set(CMAKE_CXX_EXTENSIONS OFF)

# bump up warning levels appropriately for clang, gcc & msvc
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	add_compile_options(
		-Wall -Wextra # Reasonable and standard
		-Wpedantic # Warn if non-standard C++ is used
		-Wunused # Warn on anything being unused
		#-Wconversion # Warn on type conversions that may lose data
		#-Wsign-conversion # Warn on sign conversions
		-Wshadow # Warn if a variable declaration shadows one from a parent context
	)

	#link_libraries(-fsanitize=address,undefined)
	#link_libraries(-fsanitize=undefined)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()

# cmake setup end

# MM code
add_subdirectory("framework")
add_subdirectory("systems")
add_subdirectory("screens")

