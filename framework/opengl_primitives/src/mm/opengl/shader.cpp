#include "./shader.hpp"

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

#include <string_view>

#include <mm/services/filesystem.hpp>
#include <mm/path_utils.hpp>

#include <mm/string_view_split.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"OpenGL", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("OpenGL", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"OpenGL", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"OpenGL", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("OpenGL", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("OpenGL", __VA_ARGS__)

//#include <iostream>

namespace MM::OpenGL {

Shader::Shader(uint32_t rendererID) : _rendererID(rendererID) {
}

Shader::~Shader(void) {
	glDeleteProgram(_rendererID);
}

void Shader::bind(void) const {
	glUseProgram(_rendererID);
}

void Shader::unbind(void) const {
	glUseProgram(0);
}

void Shader::setUniform1i(const std::string& name, int32_t v0) {
	glUniform1i(getUniformLocation(name), v0);
}

void Shader::setUniform1ui(const std::string& name, uint32_t v0) {
	glUniform1ui(getUniformLocation(name), v0);
}

void Shader::setUniform2ui(const std::string& name, uint32_t v0, uint32_t v1) {
	glUniform2ui(getUniformLocation(name), v0, v1);
}

void Shader::setUniform1f(const std::string& name, float v0) {
	glUniform1f(getUniformLocation(name), v0);
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2) {
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Shader::setUniform3f(const std::string& name, const glm::vec3& vec) {
	setUniform3f(name, vec.x, vec.y, vec.z);
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform4f(const std::string& name, const glm::vec4& vec) {
	setUniform4f(name, vec.x, vec.y, vec.z, vec.w);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}


void Shader::setUniform2f(const std::string& name, float v0, float v1) {
	glUniform2f(getUniformLocation(name), v0, v1);
}

void Shader::setUniform2f(const std::string& name, const glm::vec2& vec) {
	glUniform2f(getUniformLocation(name), vec.x, vec.y);
}

void Shader::setUniformMat3f(const std::string& name, const glm::mat3& matrix) {
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

// TODO: refactor this whole thing out
// FIXME: hangs if trailing whitespace
std::string Shader::parse(Engine& engine, const std::string& filePath) {
	auto& fs = engine.getService<MM::Services::FilesystemService>();

	auto handle = fs.open(filePath.c_str());
	if (!handle) {
		// TODO: log error
		return "";
	}

	std::string in_file;
	{
		auto read_count = fs.readString(handle, in_file);
		fs.close(handle);

		if (read_count == 0) {
			LOG_WARN("empty file");
			return "";
		}
	}

	// preprocessor, without the preprocessing, only does uncontitional include
	// actual "parsing" here:
	// ... #include is all we need tho

	std::string out_file;
	std::string_view in_file_view = in_file;

	// for each line
	while (true) {
		// view empty, end loop
		if (in_file_view.empty() || in_file_view.front() == '\0') { // fix random \0
			break;
		}

		auto next_nl_pos = in_file_view.find_first_of('\n');
		std::string_view line;
		if (next_nl_pos == 0) { // empty line, skip
			in_file_view = in_file_view.substr(1);
			//std::cout << "!!! empty line\n";
			out_file += '\n';
			continue;
		} else {
			// works for npos too
			line = in_file_view.substr(0, next_nl_pos);
		}

		// do line parsing

		auto original_line = line;

		// trim front
		// there should be no \n
		line.remove_prefix(std::min(line.find_first_not_of(" \t\r\n"), line.size()));
		// trim back
		// WRONG!!, but idc, idn
		//line.remove_suffix(std::min(line.find_last_not_of(" \t\r\n"), line.size()));
		//std::cout << "tline: " << line << "\n";

		if (line.empty()) {
			out_file += '\n';
			continue; // line was empty after trim
		}

		// TODO: refactor
		// TODO: add support for spaces in include paths
		// are we a preprocessor token
		if (line.at(0) == '#' && line.size() > 1) {
			// this trims, if all whitespace are delims
			auto tokens = MM::std_utils::split(line.substr(1), " \t\r\n");

			// rn only include
			if (tokens.empty()) {
				// TODO: warn
				out_file += original_line;
				out_file += '\n';
			} else if (tokens.front() == "include") {
				if (tokens.size() <= 1) {
					// error include missing path
					//continue;
					break;
				}

				std::string_view include_path = tokens[1];
				if (include_path.size() > 2 && include_path.front() == '"' && include_path.back() == '"') {
					include_path = include_path.substr(1, include_path.size()-2);
					//std::cout << "include path: " << include_path << "\n";

					std::string path;
					// try relative/absolut path
					if (include_path.front() == '/') { // absolut
						path = include_path;
					} else { // relative
						path = MM::base_path(filePath);
						assert(path.back() == '/'); // lel
						path += include_path;
					}

					if (!MM::path_shorten(path)) {
						LOG_ERROR("too many '..' in '{}'", path);
						break;
					}

					if (!fs.isFile(path.c_str())) {
						LOG_ERROR("unknown file '{}' ('{}')", include_path, path);
						break;
					}

					// prevent recursion
					if (filePath == path) {
						LOG_WARN("skipping direct recursion of '{}'", filePath);
					}

					std::string tmp_include_file = parse(engine, path);

					//std::cout << "#####in_file:\n";
					//for (const char c : tmp_include_file) {
						//std::cout << (int)c << "(" << std::hex << (int)c << std::dec << ") ";
					//}
					//std::cout << "\n";

					out_file += "// INCLUDED FILE START (";
					out_file += path;
					out_file += ")\n";

					out_file += tmp_include_file;

					out_file += "\n// INCLUDED FILE END (";
					out_file += path;
					out_file += ")\n";

					LOG_INFO("#included file '{}'", path);

					// otherwise use library path
				} else if (include_path.size() > 2 && include_path.front() == '<' && include_path.back() == '>') {
					include_path = include_path.substr(1, include_path.size()-2);
					//std::cout << "include path: " << include_path << "\n";
					// handle library path
				} else {
					LOG_ERROR("include path missing delimiters ('\"\"', '<>'): '{}'", include_path);
					//continue;
					break;
				}
			} else {
				// if not include
				out_file += original_line;
				out_file += '\n';
			}
		} else {
			out_file += original_line;
			out_file += '\n';
		}

		// was last line
		if (next_nl_pos == std::string_view::npos) {
			break;
		}

		in_file_view = in_file_view.substr(next_nl_pos+1);
	}

	//std::cout << "file after parsing:\n" << out_file;

	return out_file;
}

uint32_t Shader::compile(uint32_t type, const std::string& source) {
	uint32_t id = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int32_t res;
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* msg = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, msg);
		LOG_ERROR("failed to compile {} Shader: {}\n  source: {}", type == GL_VERTEX_SHADER ? "vertex" : "fragment", msg, source);
		return 0;
	}

	return id;
}

std::shared_ptr<Shader> Shader::createF(Engine& engine, const char* vertexShaderPath, const char* fragmentShaderPath) {
	std::string vs = parse(engine, vertexShaderPath);
	std::string fs = parse(engine, fragmentShaderPath);

	return create(vs, fs);
}

std::shared_ptr<Shader> Shader::create(const std::string& vertexShader, const std::string& fragmentShader) {
	uint32_t vs = compile(GL_VERTEX_SHADER, vertexShader);
	if (vs == 0) {
		return nullptr;
	}

	uint32_t fs = compile(GL_FRAGMENT_SHADER, fragmentShader);
	if (fs == 0) {
		glDeleteShader(vs);
		return nullptr;
	}

	uint32_t program = glCreateProgram();


	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		LOG_ERROR("Linking Shader Programs: {}", &infoLog[0]);

		// The program is useless now. So delete it.
		glDeleteProgram(program);
		return nullptr;
	}

#if 0
	// TODO: validate validates agains the current ogl state, so this might fail
	glValidateProgram(program);
	GLint isValid = 0;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &isValid);
	if (isValid == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		LOG_ERROR("Validating Shader Programs: {}", &infoLog[0]);

		// The program is useless now. So delete it.
		glDeleteProgram(program);
		return nullptr;

	}
#endif

	//todo: make some check if linking and validating actually worked
	return std::shared_ptr<Shader>(new Shader(program));
}

int32_t Shader::getUniformLocation(const std::string& name) {
	if (_uniformLocationCache.find(name) != _uniformLocationCache.end()) {
		return _uniformLocationCache[name];
	}

	int32_t location = glGetUniformLocation(_rendererID, name.c_str());

	if (location == -1) {
		LOG_WARN("uniform '{}' doesn't exist! (hint: the compiler removes unused variables)", name);
	}

	_uniformLocationCache[name] = location;

	return location;
}

int32_t Shader::getAttribLocation(const std::string& name) {
	int32_t location = glGetAttribLocation(_rendererID, name.c_str());

	if (location == -1) {
		LOG_WARN("attrib '{}' doesn't exist! (hint: the compiler removes unused variables)", name);
	}

	return location;
}

} // MM::OpenGL

