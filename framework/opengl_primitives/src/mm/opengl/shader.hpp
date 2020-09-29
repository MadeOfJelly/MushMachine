#pragma once

#include <iosfwd>
//#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

#include <glm/fwd.hpp>
#include <glm/mat3x3.hpp>

// fwd
namespace MM {
	class Engine;
}

namespace MM::OpenGL {

	class Shader {
		private:
			uint32_t _rendererID;
			std::unordered_map<std::string, int32_t> _uniformLocationCache;

		public:
			~Shader(void);

			void bind(void) const;
			void unbind(void) const;

			// set uniforms
			void setUniform1i(const std::string& name, int32_t v0);

			void setUniform1ui(const std::string& name, uint32_t v0);
			void setUniform2ui(const std::string& name, uint32_t v0, uint32_t v1);

			void setUniform1f(const std::string& name, float v0);
			void setUniform2f(const std::string& name, float v0, float v1);
			void setUniform2f(const std::string& name, const glm::vec2& vec);
			void setUniform3f(const std::string& name, float v0, float v1, float v2);
			void setUniform3f(const std::string& name, const glm::vec3& vec);
			void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
			void setUniform4f(const std::string& name, const glm::vec4& vec);

			void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
			void setUniformMat3f(const std::string& name, const glm::mat3& matrix);

			static std::shared_ptr<Shader> createF(Engine& engine, const char* vertexShaderPath, const char* fragmentShaderPath);
			static std::shared_ptr<Shader> create(const std::string& vertexShader, const std::string& fragmentShader);

		private:
			explicit Shader(uint32_t rendererID);

			static std::string parse(Engine& engine, const std::string& filePath);
			static uint32_t compile(uint32_t type, const std::string& source);

		public:
			int32_t getUniformLocation(const std::string& name);
			int32_t getAttribLocation(const std::string& name);

	};

} // MM::OpenGL

