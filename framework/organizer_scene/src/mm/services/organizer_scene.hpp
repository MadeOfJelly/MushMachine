#pragma once

#include <mm/services/scene_service_interface.hpp>

#include <chrono>

namespace MM::Services {

	// provides an ~implementation for SceneServiceInterface
	// !! IMPORTANT: relies on entt::organizer. Use it to add systems!
	class OrganizerSceneService : public SceneServiceInterface {
		private:
			std::unique_ptr<Scene> _scene;
			std::unique_ptr<Scene> _next_scene; // enqueued next scene

			using clock = std::chrono::steady_clock;
			double _accumulator = 0.0;
			std::chrono::time_point<clock> _last_time;

		public:
			const float f_delta;

			float initial_delta_factor = 1.f;

		public: // service
			explicit OrganizerSceneService(const float update_delta = 1.f/60.f) : f_delta(update_delta) {}

			const char* name(void) override { return "OrganizerSceneService"; }

			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

		public: // scene interface
			Scene& getScene(void) override { return *_scene; }

			void changeScene(std::unique_ptr<Scene>&& new_scene) override;

			// be carefull of that one
			void changeSceneNow(std::unique_ptr<Scene>&& new_scene) override;

		public:
			void sceneFixedUpdate(Engine& engine);
			void changeSceneFixedUpdate(Engine& engine);

			void updateOrganizerVertices(Scene& scene);

			void resetTime(void);
	};

} // MM::Services

