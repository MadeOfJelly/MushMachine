#include <gtest/gtest.h>

#include <mm/resource_manager.hpp>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/simple_scene.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <mm/imgui/fps_overlay.hpp>

#include <mm/imgui/file_text_editor.hpp>
#include <mm/imgui/file_shader_editor.hpp>

#include <mm/services/scene_tools.hpp>

static char* argv0;

TEST(imgui_text_edit, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>(SDL_INIT_VIDEO);
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_text_edit_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_text_edit_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();
	ASSERT_TRUE(provide_ret);

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiSceneToolsService>();

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiSceneToolsService>());

	MM::ImGuiSimpleFPSOverlay fps_overlay;
	engine.addUpdate([&](MM::Engine&) {
			fps_overlay.renderImGui();
		}
	);

	MM::FileTextEditor fte{engine};
	engine.addUpdate([&](MM::Engine&) {
			fte.renderImGui();
		}
	);

	engine.run();

	// TODO: clear asset manager

	sdl_ss.destroyWindow();
}

TEST(imgui_text_edit, shader) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>(SDL_INIT_VIDEO);
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_text_edit_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_text_edit_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();
	ASSERT_TRUE(provide_ret);

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiSceneToolsService>();

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiSceneToolsService>());

	MM::ImGuiSimpleFPSOverlay fps_overlay;
	engine.addUpdate([&](MM::Engine&) {
			fps_overlay.renderImGui();
		}
	);

	//auto& rc = engine.getScene().ctx<MM::OpenGL::RenderController>();
	//rc.registerRenderer<MM::OpenGL::Renderers::QuadRenderer>();

	MM::FileTextEditor fte{engine};
	engine.addUpdate([&](MM::Engine&) {
			fte.renderImGui();
		}
	);

	MM::FileShaderEditor fse{engine};
	engine.addUpdate([&](MM::Engine&) {
			fse.renderImGui();
		}
	);

	fte.open("shader/quad_renderer/vert.glsl");
	fse.open("shader/quad_renderer/frag.glsl");

	// TODO: a shader to display plx
	//{
		//auto& ecs = engine.getScene();

		//auto& ee = igsts->getEntityEditor();
		//ee.registerTrivial<MM::OpenGL::Renderers::QuadRenderable>("QuadRenderable");
		//ee.registerComponentCreateFn(ecs.type<MM::OpenGL::Renderers::QuadRenderable>(),
				//[](MM::EngineConfig::ECS& ecs, MM::EngineConfig::Entity e) {
					//auto& r = ecs.assign<MM::OpenGL::Renderers::QuadRenderable>(e);
					//r._texture = MM::ResourceManager<MM::OpenGL::Texture>::ref().get("default"_hs);
				//}
			//);
		//ee.registerComponentWidgetFn(ecs.type<MM::OpenGL::Renderers::QuadRenderable>(),
				//[](MM::EngineConfig::ECS& ecs, MM::EngineConfig::Entity e) {
					//auto& r = ecs.get<MM::OpenGL::Renderers::QuadRenderable>(e);
					//MM::ImGuiWidgets::Components::QuadRenderable(r);
				//}
			//);
	//}

	engine.run();

	// TODO: clear asset manager

	sdl_ss.destroyWindow();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

