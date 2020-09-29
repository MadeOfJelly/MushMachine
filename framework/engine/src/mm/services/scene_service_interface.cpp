#include "./scene_service_interface.hpp"

#include <entt/entity/registry.hpp>

namespace MM {

struct DefaultSystemsContainer {
	// list of callables, expecting a Scene (ecs) and a step size (delta)
	std::vector<std::function<void(Scene&, float)>> systems;
};

void AddSystemToScene(::MM::Scene& scene, ::MM::System fn) {
	auto& sc = scene.ctx_or_set<DefaultSystemsContainer>();
	sc.systems.emplace_back(std::move(fn));
}

void EachSystemInScene(::MM::Scene& scene, std::function<void(::MM::Scene&, ::MM::System&)> fn) {
	auto* sc = scene.try_ctx<DefaultSystemsContainer>();
	if (sc != nullptr) {
		for (auto& system : sc->systems) {
			fn(scene, system);
		}
	}
}

} // MM

