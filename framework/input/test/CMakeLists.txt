add_executable(input_service_test
	start_test.cpp
)

target_include_directories(input_service_test PRIVATE ".")

target_link_libraries(input_service_test
	input_service
	sdl_service
	gtest_main
)

add_test(NAME input_service_test COMMAND input_service_test)

######################

add_executable(input_service_visualizer_test
	input_visualizer.cpp
)

target_include_directories(input_service_visualizer_test PRIVATE ".")

target_link_libraries(input_service_visualizer_test
	input_service
	sdl_service
	imgui_service
	imgui_render_task
	imgui_tools
	gtest_main
)

add_test(NAME input_service_visualizer_test COMMAND input_service_visualizer_test)

