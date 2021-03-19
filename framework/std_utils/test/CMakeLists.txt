add_executable(std_utils_test
	scalar_range2_test.cpp
	scalar_range2_json_test.cpp
)

target_include_directories(std_utils_test PRIVATE ".")

target_link_libraries(std_utils_test
	gtest_main
	gmock

	std_utils
	std_utils_serialize
)

add_test(NAME std_utils_test COMMAND std_utils_test)

