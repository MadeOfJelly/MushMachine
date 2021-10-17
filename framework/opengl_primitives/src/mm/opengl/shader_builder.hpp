#pragma once

#include "./shader.hpp"

#include <vector>

namespace MM::OpenGL {

	class ShaderBuilder {
		private:
			ShaderBuilder(void);

			struct stage_t {
				uint32_t id = 0;
				bool fail = false;
			};

			enum stage_e {
				VERTEX = 0,
				FRAGMENT,
				stage_e_MAX
			};

			stage_t _stages[stage_e_MAX];

			struct transform_feedback_varying_t {
				std::string var;
			};
			std::vector<transform_feedback_varying_t> _tfs;

			bool _tf_interleaved = false;

		public:
			~ShaderBuilder(void);
			ShaderBuilder& operator=(ShaderBuilder&) = delete;

			static ShaderBuilder start(void);
			std::shared_ptr<Shader> finish(void);

		public:
			ShaderBuilder& addStageVertex(const std::string& shader_code);
			ShaderBuilder& addStageVertexF(MM::Engine& engine, const std::string& file_path);
			ShaderBuilder& addStageFragment(const std::string& shader_code);
			ShaderBuilder& addStageFragmentF(MM::Engine& engine, const std::string& file_path);
			// TODO: geometry and tesselation stages

			ShaderBuilder& addTransformFeedbackVarying(const std::string& var_name);
			ShaderBuilder& setTransformFeedbackInterleaved(bool interleaved = true);
	};

} // MM::OpenGL

