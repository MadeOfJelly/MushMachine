cmake_minimum_required(VERSION 3.2)
project(SquirrelNoise)

add_library(squirrel_noise
	./src/squirrel_noise/RawNoise.hpp
	./src/squirrel_noise/RawNoise.cpp

	# TODO: seperate smooth?
	./src/squirrel_noise/SmoothNoise.hpp
	./src/squirrel_noise/SmoothNoise.cpp
)

target_include_directories(squirrel_noise PUBLIC "src")

target_link_libraries(squirrel_noise
	PRIVATE glm
)

