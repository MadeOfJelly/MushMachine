#pragma once

#include "./file_text_editor.hpp"

namespace MM {

	class FileShaderEditor : public FileTextEditor {
		protected:
			TextEditor::ErrorMarkers _markers;

		protected:
			void postSave(void) override;

			void checkErrors(void);
			void handleErrorString(const char* msg);

			void handleErrorStringFormat1(const char* msg);

		public:
			FileShaderEditor(Engine& engine);

			void renderImGui(void);

		public:
			bool checkErrorsOnChange = true;

			uint32_t shaderType;
	};
} // MM

