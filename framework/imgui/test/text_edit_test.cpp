#include <functional>
#include <gtest/gtest.h>

#include <mm/resource_manager.hpp>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>
#include <mm/services/imgui_menu_bar.hpp>
#include <mm/services/engine_tools.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <mm/imgui/fps_overlay.hpp>

#include <mm/imgui/file_text_editor.hpp>
#include <mm/imgui/file_shader_editor.hpp>

static char* argv0;

using namespace entt::literals;

class TemplateUpdateMainService : public MM::Services::Service {
	std::function<void(MM::Engine&)> _fn;
	public:
		explicit TemplateUpdateMainService(std::function<void(MM::Engine&)> fn) : _fn(fn) {}

		const char* name(void) override { return "TemplateUpdateMainService"; }

		bool enable(MM::Engine&, std::vector<MM::UpdateStrategies::TaskInfo>& task_array) override {
			task_array.push_back(
				MM::UpdateStrategies::TaskInfo{"TemplateUpdateMainService::fn"}
				.fn(_fn)
			);
			return true; 
		}

		void disable(MM::Engine&) override {}
};

TEST(imgui_text_edit, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>(SDL_INIT_VIDEO);
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_text_edit_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_text_edit_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiMenuBar>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiMenuBar>());

	engine.addService<MM::Services::ImGuiEngineTools>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiEngineTools>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	engine.addService<TemplateUpdateMainService>([](MM::Engine& e) {
		static MM::FileTextEditor fte{e};
		fte.renderImGui();
	});
	ASSERT_TRUE(engine.enableService<TemplateUpdateMainService>());

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

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiMenuBar>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiMenuBar>());

	engine.addService<MM::Services::ImGuiEngineTools>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiEngineTools>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	MM::FileTextEditor fte{engine};
	MM::FileShaderEditor fse{engine};
	engine.addService<TemplateUpdateMainService>([&](MM::Engine&) {
		fte.renderImGui();
		fse.renderImGui();
	});
	ASSERT_TRUE(engine.enableService<TemplateUpdateMainService>());

	fte.open("shader/quad_renderer/vert.glsl");
	fse.open("shader/quad_renderer/frag.glsl");

	// TODO: a shader to display plx

	engine.run();

	// TODO: clear asset manager

	sdl_ss.destroyWindow();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

