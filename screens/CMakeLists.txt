cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

project(screens)

if(NOT MM_HEADLESS)
	add_subdirectory(mm_logo)
endif()

