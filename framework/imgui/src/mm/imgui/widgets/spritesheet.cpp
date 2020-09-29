#include "./spritesheet.hpp"

#include <mm/opengl/spritesheet.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "./texture.hpp"

#include <mm/logger.hpp>

namespace MM::ImGuiWidgets {

void SpriteSheetPreview(MM::OpenGL::SpriteSheet& sheet, ImVec2 size) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const ImVec2 p = ImGui::GetCursorScreenPos();

	const ImVec2 uv_a {0.f, 1.f};
	const ImVec2 uv_b {1.f, 0.f};

	draw_list->AddImage(sheet.tex->getHandle(), p, {p.x + size.x, p.y + size.y}, uv_a, uv_b);

	const ImU32 line_col = ImColor(ImVec4(1.f, 0.f, 0.f, 1.f));

	// TODO: tile size?

	// v lines
	for (uint32_t i = 0; i <= sheet.tile_count.x; i++) {
		const float scale = size.x/sheet.tile_count.x;
		const float x = p.x + scale*(float)i;
		draw_list->AddLine({x, p.y}, {x, p.y + size.y}, line_col);
	}

	// h lines
	for (uint32_t i = 0; i <= sheet.tile_count.y; i++) {
		const float scale = size.y/sheet.tile_count.y;
		const float y = p.y + scale*(float)i;;
		draw_list->AddLine({p.x, y}, {p.x + size.x, y}, line_col);
	}
}

void SpriteSheet(MM::OpenGL::SpriteSheet& sheet) {
	const float w_all = ImGui::CalcItemWidth();
	const ImVec2 dims {w_all, w_all * ((float)sheet.tex->height/sheet.tex->width)};
	SpriteSheetPreview(sheet, dims);

	// TODO: also activate by drag n drop targeting
	if (ImGui::InvisibleButton("clickability", dims)) {
		ImGui::OpenPopup("sprite_sheet_editor_popup");
	}

	if (ImGui::BeginPopup("sprite_sheet_editor_popup")) {
		SpriteSheetEditor(sheet);
		ImGui::EndPopup();
	}
}

void LabelSpriteSheet(const char* label, MM::OpenGL::SpriteSheet& sheet) {
	const char* lable_display_end = ImGui::FindRenderedTextEnd(label);

	ImGui::BeginGroup();
	ImGui::PushID(label);

	SpriteSheet(sheet);

	// drop
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_MM_REND_TEXTURE)) {
			if (payload->DataSize != sizeof(MM::ResourceManager<MM::OpenGL::Texture>::res_id_type)) {
				SPDLOG_ERROR("drag'n'drop data size missmatch ({} {})", payload->DataSize, sizeof(MM::ResourceManager<MM::OpenGL::Texture>::res_id_type));
			} else {
				auto& id = *(MM::ResourceManager<MM::OpenGL::Texture>::res_id_type*)payload->Data;
				sheet.tex = MM::ResourceManager<MM::OpenGL::Texture>::ref().get(id);
			}
		}

		ImGui::EndDragDropTarget();
	}


	ImGui::SameLine();

	//const ImVec2 label_size = ImGui::CalcTextSize(label, lable_display_end, true);

	//ImGui::Dummy({(w_all - (size.x + label_size.x) + 42.f), s_sz});

	if (label != lable_display_end) {
		ImGui::SameLine();
		ImGui::TextUnformatted(label, lable_display_end);
	}


	ImGui::PopID(); // pop label
	ImGui::EndGroup();

}

void SpriteSheetEditor(MM::OpenGL::SpriteSheet& sheet) {
	assert(sheet.tex);
	const float w_all = ImGui::CalcItemWidth();
	const ImVec2 dims {w_all, w_all * ((float)sheet.tex->height/sheet.tex->width)};
	SpriteSheetPreview(sheet, dims);
	ImGui::Dummy(dims);

	ImGui::Separator();

	MM::ImGuiWidgets::LabelTexture("tex##SpriteSheet", sheet.tex);

	ImGui::Separator();

	uint32_t step = 1;
	ImGui::InputScalar("tile_count x##SpriteSheet", ImGuiDataType_U32, &sheet.tile_count.x, &step, NULL, NULL, 0);
	ImGui::InputScalar("tile_count y##SpriteSheet", ImGuiDataType_U32, &sheet.tile_count.y, &step, NULL, NULL, 0);
}

} // MM::ImGuiWidgets

