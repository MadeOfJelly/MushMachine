#pragma once

// forward
struct ImVec2;
namespace MM::OpenGL {
	struct SpriteSheet;
}

namespace MM::ImGuiWidgets {

void SpriteSheetPreview(MM::OpenGL::SpriteSheet& sheet, ImVec2 size);
void SpriteSheet(MM::OpenGL::SpriteSheet& sheet);
void LabelSpriteSheet(const char* label, MM::OpenGL::SpriteSheet& sheet);
void SpriteSheetEditor(MM::OpenGL::SpriteSheet& sheet);

} // MM::ImGuiWidgets

