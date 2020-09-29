add_executable(player_velocity_test
	player_velocity_test.cpp
)

target_include_directories(player_velocity_test PRIVATE ".")

target_link_libraries(player_velocity_test
	player_velocity_system
	simple_scene
	gtest_main
)

add_test(NAME player_velocity_test COMMAND player_velocity_test)

