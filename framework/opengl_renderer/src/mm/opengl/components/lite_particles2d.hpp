#pragma once

#include <glm/vec2.hpp>

#include <queue> // tmp

namespace MM::Components {

	// not intended to be a component
	// see LiteParticles2DUploadQueue
	struct LiteParticle2D {
		uint32_t type_id {0u}; // entt::hashed_string, ResourceManager

		glm::vec2 pos {0.f, 0.f};
		glm::vec2 vel {0.f, 0.f};

		float age {0.f};
	};

	struct LiteParticles2DUploadQueue {
		// TODO: vector
		std::queue<LiteParticle2D> queue;
	};

} // MM::Components

