#include "./file_text_editor.hpp"

#include <mm/services/filesystem.hpp>
#include <physfs.h>

#include <algorithm>

#include <mm/imgui/widgets/filesystem.hpp>

#include <IconsIonicons.h>

#include <mm/logger.hpp>
#define LOGFTE(x) LOG("FileTextEditor", x)

namespace MM {

FileTextEditor::FileTextEditor(Engine& engine) {
	_windowID = ICON_II_CODE " File Text Editor##";
	_windowID += std::to_string(rand()); // lets hope they dont collide

	_fs_ptr = engine.tryService<MM::Services::FilesystemService>();
}

void FileTextEditor::setLanguageDefinition(const TextEditor::LanguageDefinition& lang) {
	_te_lang = lang;
	_te.SetLanguageDefinition(_te_lang);
}

void FileTextEditor::renderPopups(void) {
	if (ImGui::BeginPopupModal("Open File", nullptr,
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings)) {
		MM::ImGuiWidgets::FilePicker("open", *_fs_ptr, _tmp_file_path);

		if (ImGui::Button("Load File")) {
			bool succ = open(_tmp_file_path);

			ImGui::CloseCurrentPopup();

			if (!succ) {
				// TODO: handle error
				LOGFTE("error: opening file");
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Save File As", nullptr,
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoSavedSettings)) {
		MM::ImGuiWidgets::FilePicker("save", *_fs_ptr, _tmp_file_path, true);

		if (ImGui::Button("Save")) {
			auto old = _file_path;
			_file_path = _tmp_file_path;

			if (!save()) {
				LOGFTE(std::string("error: saving file as '") + _file_path + "'");
				_file_path = old; // retore in case of error
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void FileTextEditor::renderImGui(bool* is_open) {
	if (ImGui::Begin(_windowID.c_str(), is_open, /*ImGuiWindowFlags_HorizontalScrollbar | */ImGuiWindowFlags_MenuBar)) {
		ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

		bool open_file_open = false;
		bool open_file_save = false;

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open")) {
					open_file_open = true;
				}

				if (ImGui::MenuItem("New File")) {
					_file_path.clear();
					_tmp_file_path.clear();
					_te.SetText("");
				}

				if (ImGui::MenuItem("Save")) {
					save();
				}

				if (ImGui::MenuItem("Save as")) {
					open_file_save = true;
				}


				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				if (_te.IsReadOnly() && ImGui::MenuItem("Enable Editing")) {
					_te.SetReadOnly(false);
				}

				if (!_te.IsReadOnly() && ImGui::MenuItem("Disable Editing")) {
					_te.SetReadOnly(true);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}


		// popups
		{
			if (open_file_open)
				ImGui::OpenPopup("Open File");

			if (open_file_save)
				ImGui::OpenPopup("Save File As");

			renderPopups();
		}

		auto cpos = _te.GetCursorPosition();

		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s",
				cpos.mLine + 1,
				cpos.mColumn + 1,
				_te.GetTotalLines(),
				_te.IsOverwrite() ? "Ovr" : "Ins",
				_te.CanUndo() ? "*" : " ",
				_te.GetLanguageDefinition().mName.c_str(), _file_path.c_str());

		_te.Render("TextEditor");

	}
	ImGui::End();
}

bool FileTextEditor::open(Services::FilesystemService::fs_file_t file, bool write) {
	if (!file)
		return false;

	//auto& fs = MM::locator::filesystem_service::ref();

	_fs_ptr->seek(file, 0);

	std::string text;
	//char buffer [265];
	//while (auto r = _fs_ptr->read(file, buffer, 265)) {
		//text.append(buffer, r);
	//}
	_fs_ptr->readString(file, text);

	_te.SetText(text);

	_te.SetReadOnly(!write);

	postOpen();
	return true;
}

bool FileTextEditor::open(const std::string& path, bool write) {
	if (path.empty())
		return false;

	//auto& fs = MM::locator::filesystem_service::ref();

	auto h = _fs_ptr->open(path.c_str());
	if (!h)
		return false;

	_file_path = path;

	// lang detection
	{
		// TODO: make better
		std::string tmp = path.substr(path.size()-4);
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](unsigned char c) { return std::tolower(c); });
		if (tmp == "glsl") {
			setLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
		}
	}

	return open(h, write) && _fs_ptr->close(h); // lul
}

bool FileTextEditor::save(void) {
	if (_file_path.empty()/* || !_fs_ptr->exists(_file_path.c_str())*/)
		return false;

	// create dir in write, if not exists (bug in physfs?)
	{
		std::string tmp = _file_path.substr(0, _file_path.find_last_of('/'));
		if (!tmp.empty()) {
			if (!PHYSFS_mkdir(tmp.c_str())) {
				LOGFTE("error making dir");
			}
		}
	}

	auto h = _fs_ptr->open(_file_path.c_str(), Services::FilesystemService::WRITE);
	if (!h) {
		LOGFTE("error opening file for saving");
		return false;
	}

	auto text = _te.GetText();

	auto written = _fs_ptr->write(h, text.c_str(), text.size());
	_fs_ptr->close(h);
	if (written != text.size()) {
		LOGFTE("written size not equal to text size");
		return false;
	}

	postSave();
	return true;
}

} // MM

