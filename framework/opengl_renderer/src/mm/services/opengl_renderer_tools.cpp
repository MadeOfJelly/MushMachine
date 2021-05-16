#include "./opengl_renderer_tools.hpp"

#include <mm/services/opengl_renderer.hpp>

#include <entt/core/hashed_string.hpp>

#include <imgui/imgui.h>

#include <mm/imgui/widgets/texture_resource_manager.hpp>

namespace MM::Services {

bool ImGuiOpenGLRendererTools::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

	menu_bar.menu_tree["OpenGL"]["RenderTasks"] = [this](Engine&) {
		ImGui::MenuItem("Render Tasks", NULL, &_show_render_tasks);
	};

	menu_bar.menu_tree["OpenGL"]["TextureCacheLegacy"] = [this](Engine&) {
		ImGui::MenuItem("Texture Cache (legacy)", NULL, &_show_texture_cache_legacy);
	};

	// add task
	task_array.push_back(
		UpdateStrategies::TaskInfo{"ImGuiOpenGLRendererTools::render"}
		.fn([this](Engine& e) { renderImGui(e); })
		.phase(UpdateStrategies::update_phase_t::PRE)
		.succeed("ImGuiMenuBar::render")
	);

	return true;
}

void ImGuiOpenGLRendererTools::disable(Engine& engine) {
	auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

	menu_bar.menu_tree["OpenGL"].erase("RenderTasks");
	menu_bar.menu_tree["OpenGL"].erase("TextureCacheLegacy");
}

void ImGuiOpenGLRendererTools::renderImGui(Engine& engine) {
	if (_show_render_tasks) {
		renderRenderTasks(engine);
	}

	if (_show_texture_cache_legacy) {
		renderTextureCacheLegacy(engine);
	}
}

void ImGuiOpenGLRendererTools::renderRenderTasks(Engine& engine) {
	if (ImGui::Begin("RenderTasks##OpenGLRendererTools", &_show_render_tasks)) {
		auto& ogl_renderer = engine.getService<MM::Services::OpenGLRenderer>();

		if (ImGui::BeginTable("table", 1)) {
			for (const auto& task : ogl_renderer.render_tasks) {
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::Text("%s", task->name());
			}
			ImGui::EndTable();
		}

	}
	ImGui::End();
}

void ImGuiOpenGLRendererTools::renderTextureCacheLegacy(Engine& engine) {
	if (ImGui::Begin("TextureCache (legacy)##OpenGLRendererTools", &_show_texture_cache_legacy)) {
		if (ImGui::BeginTabBar("tabs")) {
			if (ImGui::BeginTabItem("Texture List")) {
				ImGuiWidgets::TextureResourceManagerList();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Texture Loader")) {
				ImGuiWidgets::TextureResourceManagerLoader(engine);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

} // MM::Services
