cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(random CXX)

add_library(random
	./src/mm/random/srng.hpp
	./src/mm/random/srng.cpp
)

target_include_directories(random PUBLIC "src")

target_compile_features(random PUBLIC cxx_std_17)

target_link_libraries(random
	PUBLIC
		squirrel_noise
		std_utils
)

##############################

#if (BUILD_TESTING)
	#add_subdirectory(test)
#endif()

