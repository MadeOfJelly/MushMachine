#include "texture_resource_manager.hpp"

#include <mm/imgui/widgets/filesystem.hpp>

#include <mm/services/filesystem.hpp>

#include <mm/opengl/texture_loader.hpp>

#include <imgui/imgui.h>

#include <mm/logger.hpp>

namespace MM::ImGuiWidgets {

void TextureResourceManagerList(void) {
	auto& rm = MM::ResourceManager<MM::OpenGL::Texture>::ref();

	ImGui::Text("textures is cache: %lu", rm.size());

	std::function<void(MM::ResourceManager<MM::OpenGL::Texture>::res_id_type, std::shared_ptr<MM::OpenGL::Texture>)> fn =
	[](auto /*id*/, std::shared_ptr<MM::OpenGL::Texture> h) {
		Texture(h, false);
		return;
	};

	if (ImGui::BeginChild("texture list")) {
		rm.each(fn);
	}
	ImGui::EndChild();
}

void TextureResourceManagerLoader(MM::Engine& engine) {
	using namespace entt::literals;
	auto& rm = MM::ResourceManager<MM::OpenGL::Texture>::ref();
	static MM::OpenGL::Texture::handle texture = rm.get("default"_hs);

	static std::string path = "";

	auto& fs = engine.getService<MM::Services::FilesystemService>();
	MM::ImGuiWidgets::FilePicker("texture_loader", fs, path, false);

	static char ident[30] = {};

	ImGui::InputText("texture id", ident, 29);

	bool load = false;
	bool force = false;
	ImGui::BeginGroup();
	if (ImGui::Button("load file!")) {
		load = true;
	}

	ImGui::SameLine();

	if (ImGui::Button("force load file!")) {
		load = true;
		force = true;
	}
	ImGui::EndGroup();

	if (load) {
		auto id = entt::hashed_string{ident}.value();
		if (!force && rm.contains(id)) {
			SPDLOG_ERROR("TextureResourceManagerLoader: id allready used! ({})", id);
		} else if (!force && strlen(ident) == 0) {
			SPDLOG_ERROR("TextureResourceManagerLoader: id empty!");
		} else if (!fs.isFile(path.c_str())) {
			SPDLOG_ERROR("TextureResourceManagerLoader: '{}' is not a file!", path);
		} else {
			if (!force && rm.load<MM::OpenGL::TextureLoaderFile>(ident, engine, path)) {
				texture = rm.get(id);
			} else if (force && rm.reload<MM::OpenGL::TextureLoaderFile>(ident, engine, path)) {
				texture = rm.get(id);
			} else {
				SPDLOG_ERROR("TextureResourceManagerLoader: loading texture from file! {}", path);
			}
		}
	}

	ImGui::TextUnformatted("last loaded texture:");
	ImGui::SameLine();
	MM::ImGuiWidgets::Texture(texture, false);
}

}

