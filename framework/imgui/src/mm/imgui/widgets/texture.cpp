#include "./texture.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <mm/logger.hpp>

namespace MM::ImGuiWidgets {

// TODO: fix formating
void Texture(MM::OpenGL::Texture::handle& texture, bool dropTarget) {
	const float s_sz = ImGui::GetFrameHeight();
	//const float w_all = ImGui::CalcItemWidth();
	//const char* lable_display_end = ImGui::FindRenderedTextEnd(label);
	ImVec2 size{s_sz*2.f, s_sz*2.f};

	ImGui::BeginGroup();

	if (texture) {
		ImGui::Image(texture->getHandle(), size, {0.f, 1.f}, {1.f, 0.f}, ImVec4(1,1,1,1), ImVec4(1,1,1,1));

		// "tooltip"
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			const ImVec2 orig_size {(float)texture->width, (float)texture->height};
			ImGui::Image(texture->getHandle(), orig_size, {0.f, 1.f}, {1.f, 0.f});
			ImGui::EndTooltip();
		}

		// drag
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			auto id = MM::ResourceManager<MM::OpenGL::Texture>::ref().id_from_handle(texture);
			if (id) {
				ImGui::SetDragDropPayload(IMGUI_PAYLOAD_TYPE_MM_REND_TEXTURE, &(id.value()), sizeof(id.value()));
				ImGui::Image(texture->getHandle(), size, {0.f, 1.f}, {1.f, 0.f}, ImVec4(1,1,1,1), ImVec4(1,1,1,1));
			}
			ImGui::EndDragDropSource();
		}
	} else {
		// TODO: default texture
		ImGui::TextUnformatted("NO TEXTURE");
	}

	// drop
	if (dropTarget && ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_MM_REND_TEXTURE)) {
			if (payload->DataSize != sizeof(MM::ResourceManager<MM::OpenGL::Texture>::res_id_type)) {
				SPDLOG_ERROR("drag'n'drop data size missmatch ({} {})", payload->DataSize, sizeof(MM::ResourceManager<MM::OpenGL::Texture>::res_id_type));
			} else {
				auto& id = *(MM::ResourceManager<MM::OpenGL::Texture>::res_id_type*)payload->Data;
				texture = MM::ResourceManager<MM::OpenGL::Texture>::ref().get(id);
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::EndGroup();
}

void LabelTexture(const char* label, MM::OpenGL::Texture::handle& texture, bool dropTarget) {
	const float s_sz = ImGui::GetFrameHeight();
	const float w_all = ImGui::CalcItemWidth();
	const char* lable_display_end = ImGui::FindRenderedTextEnd(label);

	ImVec2 size{s_sz*2.f, s_sz*2.f};
	if (!texture) {
		size = ImGui::CalcTextSize("NO TEXTURE");
	}

	ImGui::BeginGroup();
	ImGui::PushID(label);

	Texture(texture, dropTarget);

	ImGui::SameLine();

	const ImVec2 label_size = ImGui::CalcTextSize(label, lable_display_end, true);

	ImGui::Dummy({(w_all - (size.x + label_size.x) + 42.f), s_sz});

	if (label != lable_display_end) {
		ImGui::SameLine();
		ImGui::TextUnformatted(label, lable_display_end);
	}


	ImGui::PopID(); // pop label
	ImGui::EndGroup();
}

}

