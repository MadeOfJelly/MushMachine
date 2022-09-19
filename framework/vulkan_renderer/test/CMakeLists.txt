add_executable(vulkan_renderer_test
	./vulkan_test.cpp
)

target_include_directories(vulkan_renderer_test PRIVATE ".")

target_link_libraries(vulkan_renderer_test
	vulkan_renderer
	gtest_main
)

add_test(NAME vulkan_renderer_test COMMAND vulkan_renderer_test)

