add_executable(screen_director_test
	sd_test.cpp
)

target_include_directories(screen_director_test PRIVATE ".")

target_link_libraries(screen_director_test
	screen_director
	gtest_main
)

add_test(NAME screen_director_test COMMAND screen_director_test)

