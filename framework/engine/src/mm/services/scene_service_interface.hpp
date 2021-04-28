#pragma once

#include <mm/engine.hpp>

#include <entt/entity/fwd.hpp>

namespace MM {

	using Entity = entt::entity;
	using Scene = entt::basic_registry<::MM::Entity>;
	using System = std::function<void(::MM::Scene&, float)>;

	// opaque way to add a System to a Scene
	[[deprecated("use organizer")]]
	void AddSystemToScene(::MM::Scene& scene, ::MM::System fn);

	// opaque way to iterate over the Systems
	[[deprecated("use organizer")]]
	void EachSystemInScene(::MM::Scene& scene, std::function<void(::MM::Scene&, ::MM::System&)> fn);

} // MM

namespace MM::Services {

	class SceneServiceInterface : public Service {
		public: // Scene stuff
			// get current Scene
			virtual ::MM::Scene& getScene(void) = 0;

			// enques a new Scene to be put in place
			virtual void changeScene(std::unique_ptr<::MM::Scene>&& new_scene) = 0;

			// sets the new Scene to be provided.
			// dont use, except for when you know what you are doing!
			// be carefull of that one (lol)
			virtual void changeSceneNow(std::unique_ptr<::MM::Scene>&& new_scene) = 0;

			// adds a System to current Scene.
			// default impl. will use getScene() !
			virtual void addSystemToScene(::MM::System fn) {
				::MM::AddSystemToScene(getScene(), std::move(fn));
			}
	};

} // MM::Services

