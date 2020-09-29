add_executable(simple_velocity_test
	simple_velocity_test.cpp
)

target_include_directories(simple_velocity_test PRIVATE ".")

target_link_libraries(simple_velocity_test
	simple_velocity_system
	simple_scene
	gtest_main
)

add_test(NAME simple_velocity_test COMMAND simple_velocity_test)

