#pragma once

#include <mm/services/scene_service_interface.hpp>

#include <chrono>

namespace MM::Services {

	// provides an implementation for SceneServiceInterface
	class SimpleSceneService : public SceneServiceInterface {
		private:
			std::unique_ptr<Scene> _scene;
			std::unique_ptr<Scene> _next_scene; // enqueued next scene

			using clock = std::chrono::high_resolution_clock;
			long long int _accumulator = 0;
			std::chrono::time_point<clock> _last_time;

		public:
			const float f_delta;

			float delta_factor = 1.f;

		public:
			explicit SimpleSceneService(const float update_delta = 1.f/60.f) : f_delta(update_delta) {}

			const char* name(void) override { return "SimpleSceneService"; }

			bool enable(Engine& engine) override;
			void disable(Engine& engine) override;

			std::vector<UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override;

		private:
			void sceneFixedUpdate(Engine& engine);
			void changeSceneFixedUpdate(Engine& engine);

		public:
			Scene& getScene(void) override { return *_scene; }

			void changeScene(std::unique_ptr<Scene>&& new_scene) override;

			// be carefull of that one
			void changeSceneNow(std::unique_ptr<Scene>&& new_scene) override;

			void resetTime(void);
	};

} // MM::Services

