add_executable(engine_test
	update_strategy_test.cpp
	default_us_test.cpp
	#dependency_check_us_test.cpp

	# old:
	#update_test.cpp
	#run_test.cpp
	#service_test.cpp
	#default_service_test.cpp
)

target_include_directories(engine_test PRIVATE ".")

target_link_libraries(engine_test
	engine
	gtest_main
	gmock
)

add_test(NAME engine_test COMMAND engine_test)

