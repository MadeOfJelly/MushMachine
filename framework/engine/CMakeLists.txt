cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(engine CXX)

add_library(engine
	src/mm/engine_fwd.hpp
	src/mm/engine.hpp
	src/mm/engine.cpp

	src/mm/update_strategies/update_strategy.hpp
	src/mm/update_strategies/dummy.hpp

	src/mm/services/service.hpp

	src/mm/update_strategies/sequential_strategy.hpp
	src/mm/update_strategies/sequential_strategy.cpp

	src/mm/services/scene_service_interface.hpp

	src/mm/services/net_channeled_interface.hpp
)

# find . -type f -exec sed -i 's/simple_scene\/services\/simple_scene_ss.hpp/services\/scene_service_interface.hpp/g' {} \;

target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(engine
	tracy_client
	logger
	entt
	glm
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

