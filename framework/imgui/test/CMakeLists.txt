add_executable(imgui_json_test
	./json_editor_test.cpp
)
target_include_directories(imgui_json_test PRIVATE ".")
target_link_libraries(imgui_json_test
	engine
	sdl_service
	opengl_renderer_s
	imgui_service
	imgui_render_task
	imgui_widgets
	nlohmann_json::nlohmann_json
	gtest_main
)
add_test(NAME imgui_json_test COMMAND imgui_json_test)

####################

add_executable(imgui_widget_test
	./widget_test.cpp
)
target_include_directories(imgui_widget_test PRIVATE ".")
target_link_libraries(imgui_widget_test
	engine
	sdl_service
	opengl_renderer_s
	imgui_service
	imgui_render_task
	imgui_widgets
	gtest_main
)
add_test(NAME imgui_widget_test COMMAND imgui_widget_test)

####################

add_executable(imgui_scene_tools_test
	scene_tools_test.cpp
	#res/res_errig.zip.h
	#res/res_default.zip.h
)

target_include_directories(imgui_scene_tools_test PRIVATE ".")

target_link_libraries(imgui_scene_tools_test
	engine
	simple_scene
	opengl_renderer_s
	imgui_service
	imgui_render_task
	imgui_tools
	gtest_main
)

add_test(NAME imgui_scene_tools_test COMMAND imgui_scene_tools_test)

####################

add_executable(imgui_engine_tools_test
	engine_tools_test.cpp
)

target_include_directories(imgui_engine_tools_test PRIVATE ".")

target_link_libraries(imgui_engine_tools_test
	engine
	opengl_renderer_s
	imgui_service
	imgui_render_task
	imgui_tools
	gtest_main
)

add_test(NAME imgui_engine_tools_test COMMAND imgui_engine_tools_test)

####################

add_executable(imgui_sound_test
	sound_test.cpp
)
target_include_directories(imgui_sound_test PRIVATE ".")
target_link_libraries(imgui_sound_test
	engine
	sdl_service
	opengl_renderer_s
	imgui_service
	imgui_render_task
	imgui_sound
	imgui_tools
	gtest_main
)
add_test(NAME imgui_sound_test COMMAND imgui_sound_test)

####################

add_executable(imgui_text_edit_test
	./text_edit_test.cpp
)

target_include_directories(imgui_text_edit_test PRIVATE ".")

target_link_libraries(imgui_text_edit_test
	engine
	simple_scene
	opengl_renderer_s
	imgui_service
	imgui_render_task
	imgui_tools
	gtest_main
)

add_test(NAME imgui_text_edit_test COMMAND imgui_text_edit_test)

