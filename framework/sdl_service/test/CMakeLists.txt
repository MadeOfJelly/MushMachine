add_executable(sdl_service_test start_test.cpp)

target_include_directories(sdl_service_test PRIVATE ".")

target_link_libraries(sdl_service_test
	sdl_service
	gtest_main
)

add_test(NAME sdl_service_test COMMAND sdl_service_test)

