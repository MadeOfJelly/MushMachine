#include "mm/opengl/render_tasks/clear.hpp"
#include "mm/opengl/render_tasks/copy_to_fb.hpp"
#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/simple_scene.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>

#include <entt/entity/registry.hpp>

#include <mm/opengl/render_tasks/simple_rect.hpp>
#include <mm/opengl/render_tasks/blur.hpp>
#include <mm/opengl/render_tasks/imgui.hpp>

#include <mm/components/transform2d.hpp>
#include <mm/components/color.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>

#include <mm/opengl/fbo_builder.hpp>
#include <mm/opengl/texture_loader.hpp>

#include <imgui/imgui.h>

#include <random>

const char* argv0;

TEST(blur_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("blur_render_task_test", 1280, 720);

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "blur_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	//rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleRect>(engine).target_fbo = "game_view";

	rs.addRenderTask<MM::OpenGL::RenderTasks::Clear>(engine).target_fbo = "blur_io";

	MM::OpenGL::RenderTasks::SimpleRect& bsrr_rend = rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleRect>(engine);
	bsrr_rend.target_fbo = "blur_io";

	MM::OpenGL::RenderTasks::Blur& blur_rend = rs.addRenderTask<MM::OpenGL::RenderTasks::Blur>(engine);
	blur_rend.io_tex = "blur_io";
	blur_rend.temp_tex = "blur_temp";
	blur_rend.io_fbo = "blur_io";
	blur_rend.temp_fbo = "blur_temp";

	// render to display
	auto& ctfb = rs.addRenderTask<MM::OpenGL::RenderTasks::CopyToFB>(engine);
	ctfb.src_tex = "blur_io";
	ctfb.target_fbo = "display";

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	auto [w, h] = sdl_ss.getWindowSize();
	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
	{ // setup textures
		rm_t.load<MM::OpenGL::TextureLoaderEmpty>(
			"blur_io",
			GL_RGB,
			w, h,
			GL_RGB, GL_UNSIGNED_BYTE
		);

		rm_t.load<MM::OpenGL::TextureLoaderEmpty>(
			"blur_temp",
			GL_RGB,
			w, h,
			GL_RGB, GL_UNSIGNED_BYTE
		);
	}


	{ // setup fbo s
		//rs.targets["game_view"] = MM::OpenGL::FBOBuilder::start()
			//.attachTexture(MM::OpenGL::Texture::createEmpty(GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE))
			//.finish();
		rs.targets["blur_io"] = MM::OpenGL::FBOBuilder::start()
			.attachTexture(rm_t.get("blur_io"_hs))
			.setResize(true)
			.finish();
		rs.targets["blur_temp"] = MM::OpenGL::FBOBuilder::start()
			.attachTexture(rm_t.get("blur_temp"_hs))
			.setResize(true)
			.finish();
	}


	// setup v system
	MM::AddSystemToScene(scene, MM::Systems::SimpleVelocity);

	std::mt19937 mt(42);

	for (int i = 0; i < 10; i++) {
		auto e = scene.create();
		auto& t = scene.emplace<MM::Components::Transform2D>(e);
		t.position.x = float(i) * 9.f - 40.f;
		t.scale = {5,5};

		auto& v = scene.emplace<MM::Components::Velocity2D>(e);
		v.rotation = float(i) * 0.3f;

		if (mt() % 2) {
			auto& col = scene.emplace<MM::Components::Color>(e);
			auto rc = [&mt]() -> float {
				return (mt() % 1001) / 1000.f ;
			};
			col.color = {rc(),rc(),rc(),1};
		}

	}

	engine.run();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

