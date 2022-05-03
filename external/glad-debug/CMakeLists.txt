cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(glad C)

set(C_FILES
	"src/glad.c"
)

set(H_FILES
	"include/glad/glad.h"
	"include/KHR/khrplatform.h"
)

add_library(glad ${C_FILES} ${H_FILES})

target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(UNIX)
	target_link_libraries(glad dl)
endif()

