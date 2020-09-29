add_executable(filesystem_service_test
	filesystem_tests.cpp
	res/test.zip.h
)

target_include_directories(filesystem_service_test PRIVATE ".")

target_link_libraries(filesystem_service_test
	filesystem_service
	gtest_main
)

add_test(NAME filesystem_service_test COMMAND filesystem_service_test)

