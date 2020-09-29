#include "./shader.hpp"

#include <map>

#include <mm/services/filesystem.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"OpenGL", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("OpenGL", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"OpenGL", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"OpenGL", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("OpenGL", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("OpenGL", __VA_ARGS__)

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

std::string Shader::parse(Engine& engine, const std::string& filePath) {
	auto& fs = engine.getService<MM::Services::FilesystemService>();

	auto handle = fs.open(filePath.c_str());
	if (!handle) {
		// TODO: log error
		return "";
	}

	std::string file;
	fs.readString(handle, file);

	fs.close(handle);
	return file;
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
		LOG_ERROR("failed to compile {} Shader: {}", type == GL_VERTEX_SHADER ? "vertex" : "fragment", msg);
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

