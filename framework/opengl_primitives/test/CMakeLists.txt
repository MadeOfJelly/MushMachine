add_executable(opengl_fbo_test fbo_test.cpp)

target_include_directories(opengl_fbo_test PRIVATE ".")

target_link_libraries(opengl_fbo_test
	opengl_primitives
	sdl_service
	gtest_main
)

add_test(NAME opengl_fbo_test COMMAND opengl_fbo_test)

