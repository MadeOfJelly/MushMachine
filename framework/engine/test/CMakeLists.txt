add_executable(engine_test
	update_test.cpp
	run_test.cpp
	service_system_test.cpp
	default_service_test.cpp
)

target_include_directories(engine_test PRIVATE ".")

target_link_libraries(engine_test
	engine
	gtest_main
)

add_test(NAME engine_test COMMAND engine_test)

