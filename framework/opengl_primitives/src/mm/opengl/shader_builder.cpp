#include "./shader_builder.hpp"

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

#include <mm/services/filesystem.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"OpenGL", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("OpenGL", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"OpenGL", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"OpenGL", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("OpenGL", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("OpenGL", __VA_ARGS__)

namespace MM::OpenGL {

ShaderBuilder::ShaderBuilder(void) {
}

ShaderBuilder::~ShaderBuilder(void) {
	// after linking or in case of error, stages can be deleted
	for (auto& stage : _stages) {
		if (stage.id != 0) {
			glDeleteShader(stage.id);
		}
	}
}

ShaderBuilder ShaderBuilder::start(void) {
	return {};
}

std::shared_ptr<Shader> ShaderBuilder::finish(void) {
	// check
	for (auto& stage : _stages) {
		if (stage.fail) {
			// log error
			LOG_ERROR("error in shader stage");
			return nullptr;
		}
	}

	uint32_t program = glCreateProgram();

	// attach
	for (auto& stage : _stages) {
		if (stage.id != 0) {
			glAttachShader(program, stage.id);
		}
	}

	// tf varying
	if (!_tfs.empty()) {
		std::vector<const char*> tmp_vars;

		for (auto& tfv : _tfs) {
			tmp_vars.push_back(tfv.var.c_str());
		}

		glTransformFeedbackVaryings(program, tmp_vars.size(), tmp_vars.data(), _tf_interleaved ? GL_INTERLEAVED_ATTRIBS : GL_SEPARATE_ATTRIBS);

	}

	glLinkProgram(program);

	// diagnostics
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
		LOG_ERROR("Linking Shader Programs: {}", infoLog.data());

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
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
		LOG_ERROR("Validating Shader Programs: {}", infoLog.data());

		// The program is useless now. So delete it.
		glDeleteProgram(program);
		return nullptr;

	}

	return std::shared_ptr<Shader>(new Shader(program));
}

ShaderBuilder& ShaderBuilder::addStageVertex(const std::string& shader_code) {
	_stages[VERTEX].id = Shader::compile(GL_VERTEX_SHADER, shader_code);
	if (_stages[VERTEX].id == 0) {
		_stages[VERTEX].fail = true;
	}

	return *this;
}

ShaderBuilder& ShaderBuilder::addStageVertexF(MM::Engine& engine, const std::string& file_path) {
	return addStageVertex(Shader::parse(engine, file_path));
}

ShaderBuilder& ShaderBuilder::addStageFragment(const std::string& shader_code) {
	_stages[FRAGMENT].id = Shader::compile(GL_FRAGMENT_SHADER, shader_code);
	if (_stages[FRAGMENT].id == 0) {
		_stages[FRAGMENT].fail = true;
	}

	return *this;
}

ShaderBuilder& ShaderBuilder::addStageFragmentF(MM::Engine& engine, const std::string& file_path) {
	return addStageFragment(Shader::parse(engine, file_path));
}

ShaderBuilder& ShaderBuilder::addTransformFeedbackVarying(const std::string& var_name) {
	if (var_name.empty()) {
		// log waring
		return *this;
	}

	_tfs.push_back({var_name});

	return *this;
}

} // MM::OpenGL

