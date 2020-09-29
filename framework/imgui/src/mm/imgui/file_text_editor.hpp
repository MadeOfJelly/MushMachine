#pragma once

#include <TextEditor.h>

#include <mm/services/filesystem.hpp>

namespace MM {

	class FileTextEditor {
		protected:
			TextEditor _te;
			TextEditor::LanguageDefinition _te_lang;

			std::string _file_path;
			std::string _tmp_file_path;

			std::string _windowID;

			Services::FilesystemService* _fs_ptr = nullptr;

		protected:
			bool open(Services::FilesystemService::fs_file_t file, bool write = true);

			void renderPopups(void);

			virtual void postSave(void) {}
			virtual void postOpen(void) {}

		public:
			FileTextEditor(Engine& engine);
			virtual ~FileTextEditor(void) {}

			void renderImGui(bool* is_open = nullptr);

			void setLanguageDefinition(const TextEditor::LanguageDefinition& lang);

			bool open(const std::string& path, bool write = true);

			bool save(void);
	};
} // MM

