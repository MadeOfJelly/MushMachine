add_executable(std_utils_test
	scalar_range2_test.cpp
)

target_include_directories(std_utils_test PRIVATE ".")

target_link_libraries(std_utils_test
	gtest_main
	gmock

	std_utils
)

add_test(NAME std_utils_test COMMAND std_utils_test)

