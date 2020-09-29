#include "./file_shader_editor.hpp"

#include <sstream>

#ifdef MM_OPENGL_3_GLES
	//#include <SDL_opengles2_gl2.h>
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

#include <IconsIonicons.h>

#include <cstdlib>
//#include <charconv>

namespace MM {

FileShaderEditor::FileShaderEditor(Engine& engine) : FileTextEditor(engine) {
	setLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
	shaderType = GL_FRAGMENT_SHADER;

	_windowID = ICON_II_CODE " " ICON_II_IMAGE " File Shader Text Editor##";
	_windowID += std::to_string(rand()); // lets hope they dont collide
}

void FileShaderEditor::postSave(void) {
	checkErrors();
}

void FileShaderEditor::checkErrors(void) {
	_markers.clear();
	_te.SetErrorMarkers(_markers);

	auto source = _te.GetText();
	const char* src = source.c_str();

	uint32_t id = glCreateShader(shaderType);
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int32_t res;
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	int length;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
	if (length) {
		char* msg = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, msg);
		handleErrorString(msg);
	}

	glDeleteShader(id);
}

void FileShaderEditor::handleErrorString(const char* msg) {
	// TODO: detect compiler info format
	handleErrorStringFormat1(msg);
}

// accepts error messages in this format
// msgs separated by newline
// [number?]:[line number](char in line): [type eg. "error"]: [error text]
// TODO: has alot of bugs, rework
void FileShaderEditor::handleErrorStringFormat1(const char* msg) {
	std::istringstream stream;
	stream.str(msg);

	for (std::string line; std::getline(stream, line);) {
		std::string_view view(line);


		view.remove_prefix(view.find(':')+1);
		auto next_pos = view.find('(');
		char* ptr = nullptr;
		unsigned int line_number = ::strtoul(&view[0], &ptr, 10);
		if (ptr != &view[next_pos]) {
			//std::cout << "error" << std::endl;
			continue;
		}
		// no support yet :(
		//unsigned int line_number = std::from_chars(&view[0], &view[next_pos-1], 10);
		view.remove_prefix(next_pos);

		_markers.insert(std::make_pair(line_number, view));
	}

	_te.SetErrorMarkers(_markers);
}

void FileShaderEditor::renderImGui(void) {
	FileTextEditor::renderImGui();

	// append to window
	if (ImGui::Begin(_windowID.c_str())) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Shader")) {
				if (ImGui::BeginMenu("Error Checks")) {
					if (ImGui::MenuItem("Toggle Live Error Checks", nullptr, checkErrorsOnChange)) {
						checkErrorsOnChange = !checkErrorsOnChange;
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Shader Type")) {
					if (ImGui::MenuItem("Vertex Shader", nullptr, shaderType == GL_VERTEX_SHADER)) {
						shaderType = GL_VERTEX_SHADER;
					}
					if (ImGui::MenuItem("Fragment Shader", nullptr, shaderType == GL_FRAGMENT_SHADER)) {
						shaderType = GL_FRAGMENT_SHADER;
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}
	ImGui::End();


	if(checkErrorsOnChange && _te.IsTextChanged()) {
		checkErrors();
	}
}

} // MM

