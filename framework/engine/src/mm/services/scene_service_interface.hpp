#pragma once

#include <mm/engine.hpp>

#include <entt/entity/fwd.hpp>

namespace MM {

	using Entity = entt::entity;
	using Scene = entt::basic_registry<::MM::Entity>;
	using System = std::function<void(::MM::Scene&, float)>;

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
	};

} // MM::Services

