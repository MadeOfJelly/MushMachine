add_executable(opengl_renderer_s_test ./opengl_renderer_s_test.cpp)

target_include_directories(opengl_renderer_s_test PRIVATE ".")

target_link_libraries(opengl_renderer_s_test
	opengl_renderer_s
	gtest_main
)

add_test(NAME opengl_renderer_s_test COMMAND opengl_renderer_s_test)

#################

add_executable(imgui_render_task_test imgui_render_task_test.cpp)

target_include_directories(imgui_render_task_test PRIVATE ".")

target_link_libraries(imgui_render_task_test
	opengl_renderer_s
	imgui_service
	imgui_render_task
	gtest_main
)

add_test(NAME imgui_render_task_test COMMAND imgui_render_task_test)

################# simple rect render task

add_executable(simple_rect_render_task_test simple_rect_render_task_test.cpp)

target_include_directories(simple_rect_render_task_test PRIVATE ".")

target_link_libraries(simple_rect_render_task_test
	opengl_renderer_s
	simple_scene
	simple_rect_render_task

	simple_velocity_system

	gtest_main
)

add_test(NAME simple_rect_render_task_test COMMAND simple_rect_render_task_test)

################# simple sprite render task

add_executable(simple_sprite_render_task_test simple_sprite_render_task_test.cpp)

target_include_directories(simple_sprite_render_task_test PRIVATE ".")

target_link_libraries(simple_sprite_render_task_test
	opengl_renderer_s
	simple_scene
	simple_sprite_render_task

	simple_velocity_system

	gtest_main
)

add_test(NAME simple_sprite_render_task_test COMMAND simple_sprite_render_task_test)

################# simple spritesheet render task

add_executable(simple_spritesheet_render_task_test simple_spritesheet_render_task_test.cpp)

target_include_directories(simple_spritesheet_render_task_test PRIVATE ".")

target_link_libraries(simple_spritesheet_render_task_test
	opengl_renderer_s
	simple_scene
	simple_spritesheet_render_task

	#simple_velocity_system

	gtest_main
)

add_test(NAME simple_spritesheet_render_task_test COMMAND simple_spritesheet_render_task_test)

################# batched spritesheet render task

add_executable(batched_spritesheet_render_task_test batched_spritesheet_render_task_test.cpp)

target_include_directories(batched_spritesheet_render_task_test PRIVATE ".")

target_link_libraries(batched_spritesheet_render_task_test
	opengl_renderer_s
	simple_scene
	batched_spritesheet_render_task

	#simple_velocity_system

	gtest_main
)

add_test(NAME batched_spritesheet_render_task_test COMMAND batched_spritesheet_render_task_test)

#################

add_executable(blur_render_task_test blur_render_task_test.cpp)

target_include_directories(blur_render_task_test PRIVATE ".")

target_link_libraries(blur_render_task_test
	opengl_renderer_s
	simple_scene
	simple_rect_render_task
	blur_render_task
	copy_to_fb_render_task
	clear_render_task
	imgui_service
	imgui_render_task

	simple_velocity_system

	gtest_main
)

add_test(NAME blur_render_task_test COMMAND blur_render_task_test)

################# tilemap render task

add_executable(tilemap_render_task_test tilemap_render_task_test.cpp)

target_include_directories(tilemap_render_task_test PRIVATE ".")

target_link_libraries(tilemap_render_task_test
	opengl_renderer_s
	simple_scene
	imgui_service
	tilemap_render_task

	gtest_main
)

add_test(NAME tilemap_render_task_test COMMAND tilemap_render_task_test)

################# fast sky render task

add_executable(fast_sky_render_task_test fast_sky_render_task_test.cpp)

target_include_directories(fast_sky_render_task_test PRIVATE ".")

target_link_libraries(fast_sky_render_task_test
	opengl_renderer_s
	simple_scene
	#imgui_service
	fast_sky_render_task
	fast_sky_sun_system

	gtest_main
)

add_test(NAME fast_sky_render_task_test COMMAND fast_sky_render_task_test)

