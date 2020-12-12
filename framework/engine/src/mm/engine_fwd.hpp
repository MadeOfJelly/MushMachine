#pragma once
// this is a forwarding header

#include <entt/fwd.hpp>

namespace MM {
	namespace UpdateStrategies { class UpdateStrategy; }
	class Engine;
	using Entity = entt::entity;
	using Scene = entt::basic_registry<Entity>;
}

