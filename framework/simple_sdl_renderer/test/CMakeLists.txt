add_executable(simple_sdl_renderer_service_test
	start_test.cpp
)

target_include_directories(simple_sdl_renderer_service_test PRIVATE ".")

target_link_libraries(simple_sdl_renderer_service_test
	simple_sdl_renderer_service
	sdl_service
	gtest_main
)

add_test(NAME simple_sdl_renderer_service_test COMMAND simple_sdl_renderer_service_test)

