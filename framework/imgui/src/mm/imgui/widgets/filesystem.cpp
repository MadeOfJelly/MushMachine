#include "./filesystem.hpp"

#include <imgui/imgui.h>
#include <IconsIonicons.h>

#include <physfs.h>

namespace MM::ImGuiWidgets {

// also removes leafs
static std::string internal_remove_last_folder(const std::string& path) {
	if (path.empty())
		return path;

	if (path == "/")
		return path;

	auto pos = path.find_last_of('/');
	if (pos+1 == path.length()) {
		pos = path.find_last_of('/', pos-1);
	}

	//path = path.substr(0, pos+1);
	return path.substr(0, pos+1);
}

void FilePicker(const char* label, MM::Services::FilesystemService& fs, std::string& path, bool save) {
	ImGui::PushID(label);
	if (ImGui::Button("pick file")) {
		ImGui::OpenPopup("file dialogue");
	}

	ImGui::SameLine();
	ImGui::Text("\"%s\"", path.c_str());

	if (ImGui::BeginPopup("file dialogue")) {
		if (path.empty()) {
			path = "/";
		}

		ImGui::Text("path: %s", path.c_str());
		ImGui::Separator();

		std::string dirlist_path = path;

		if (dirlist_path.back() != '/')
			dirlist_path = internal_remove_last_folder(dirlist_path);

		//ImGui::TextUnformatted(dirlist_path.c_str());

		if (dirlist_path != "/") {
			if (ImGui::Selectable(ICON_II_FOLDER " ..", false, ImGuiSelectableFlags_DontClosePopups)) {
				path = internal_remove_last_folder(dirlist_path);
			}
		}

		// list folders and files
		fs.forEachIn(dirlist_path.c_str(), [&](const char* i) -> bool {
			std::string tmp_text;
			std::string tmp_path = dirlist_path + i;
			PHYSFS_Stat stat;
			PHYSFS_stat(tmp_path.c_str(), &stat);

			if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
				tmp_text += ICON_II_FOLDER " ";
			} else {
				tmp_text += ICON_II_DOCUMENT " ";
			}

			tmp_text += i;

			if (ImGui::Selectable(tmp_text.c_str(), false, ImGuiSelectableFlags_DontClosePopups)) {
				if (path.back() != '/') {
					path = internal_remove_last_folder(path);
				}

				path += i;

				if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
					path += '/';
				} else if (!save) {
					ImGui::CloseCurrentPopup();
				}
			}

			return true;
		});

		if (save) {
			ImGui::Separator();

			// TODO: perf?
			char buffer[201] = {};

			std::string path_leaf = path.substr(path.find_last_of('/')+1);
			strcpy(buffer, path_leaf.c_str());

			if (ImGui::InputText("file name", buffer, 200)) {
				// got input ?

				if (path.back() != '/')
					path = internal_remove_last_folder(path);

				path += buffer;
			}

			if (ImGui::Button("confirm")) {
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();
}

}

