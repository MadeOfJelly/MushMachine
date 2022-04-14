#pragma once

#include "./lite_particles2d_type.hpp"

#include <mm/engine_fwd.hpp>

namespace MM::OpenGL {

struct LiteParticles2DTypeLoaderJson final {
	std::shared_ptr<LiteParticles2DType> load(const nlohmann::json& j) const;
};

struct LiteParticles2DTypeLoaderFile final {
	std::shared_ptr<LiteParticles2DType> load(MM::Engine& engine, const std::string& path) const;
};

} // MM::OpenGL

