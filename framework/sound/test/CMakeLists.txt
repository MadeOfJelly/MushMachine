add_executable(sound_service_test start_test.cpp)
target_include_directories(sound_service_test PRIVATE ".")
target_link_libraries(sound_service_test
	sound_service
	gtest_main
)
add_test(NAME sound_service_test COMMAND sound_service_test)

add_executable(filesystem_soloud_file_test file_test.cpp loader_test.cpp)
target_link_libraries(filesystem_soloud_file_test
	resource_manager
	sound_service
	filesystem_service
	gtest_main

	filesystem_soloud_file
)
add_test(NAME filesystem_soloud_file_test COMMAND filesystem_soloud_file_test)

