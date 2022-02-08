#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/input_service.hpp>
#include <mm/services/organizer_scene.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/systems/player_velocity2d_system.hpp>

TEST(player_velocity, basic_run) {
	float delta = 1/60.f;
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	engine.addService<MM::Services::InputService>();
	ASSERT_TRUE(engine.enableService<MM::Services::InputService>());

	engine.addService<MM::Services::OrganizerSceneService>(delta);
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);

	auto& scene = engine.getService<MM::Services::SceneServiceInterface>().getScene();

	// setup v system
	auto& org = scene.set<entt::organizer>();
	org.emplace<&MM::Systems::player_velocity2d>("player_velocity2d");

	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);


	//auto [e, t, v] = scene.create<MM::Components::Transform, MM::Components::Velocity>();
	//t.position = { 0.f, 0.f };
	//t.rotation = 0.f;

	//v.velocity = { 1.f, 1.f };
	//v.rotation = 0.f;

	//engine.getUpdateStrategy().addDeferred([](auto& e) { e.stop(); });
	//engine.run();
	engine.update();

	//ASSERT_EQ(t.position.x, 1.f * delta);
	// TODO: TEST
}

