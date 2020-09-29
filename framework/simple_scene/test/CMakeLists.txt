add_executable(simple_scene_test
	ss_test.cpp
)

target_include_directories(simple_scene_test PRIVATE ".")

target_link_libraries(simple_scene_test
	engine
	simple_scene
	gtest_main
)

add_test(NAME simple_scene_test COMMAND simple_scene_test)

