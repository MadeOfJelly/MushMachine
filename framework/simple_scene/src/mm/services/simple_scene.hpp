#pragma once

#include <mm/services/scene_service_interface.hpp>

namespace MM::Services {

	// provides an implementation for SceneServiceInterface
	class SimpleSceneService : public SceneServiceInterface {
		private:
			std::unique_ptr<Scene> _scene;
			std::unique_ptr<Scene> _next_scene; // enqueued next scene

			MM::Engine::FunctionDataHandle _f_update_handle;
			MM::Engine::FunctionDataHandle _f_update_changer_handle;

		public:
			const char* name(void) override { return "SimpleSceneService"; }

			bool enable(Engine& engine) override;
			void disable(Engine& engine) override;

			void sceneFixedUpdate(Engine& engine);
			void changeSceneFixedUpdate(Engine& engine);

		public:
			Scene& getScene(void) override { return *_scene; }

			void changeScene(std::unique_ptr<Scene>&& new_scene) override;

			// be carefull of that one
			void changeSceneNow(std::unique_ptr<Scene>&& new_scene) override;
	};

} // MM::Services

