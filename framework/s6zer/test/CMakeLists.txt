add_executable(s6zer_test
	test.cpp
)

target_include_directories(s6zer_test PRIVATE ".")

target_compile_features(s6zer_test PRIVATE cxx_std_17)

target_link_libraries(s6zer_test
	gtest_main

	s6zer

	random
)

add_test(NAME s6zer_test COMMAND s6zer_test)

