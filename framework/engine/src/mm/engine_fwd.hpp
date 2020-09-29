#pragma once

// this is a forwarding header

#include <entt/fwd.hpp>

namespace MM {
	class Engine;
	using Entity = entt::entity;
	using Scene = entt::basic_registry<Entity>;
}

