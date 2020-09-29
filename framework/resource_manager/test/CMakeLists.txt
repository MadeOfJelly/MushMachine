add_executable(resource_test
	resource_test.cpp
)

target_include_directories(resource_test PRIVATE ".")

target_link_libraries(resource_test
	#engine
	resource_manager
	gtest_main
)

add_test(NAME resource_test COMMAND resource_test)

