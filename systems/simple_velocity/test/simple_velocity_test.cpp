#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/simple_scene.hpp>

#include <entt/entity/registry.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>

TEST(simple_velocity_2d, basic_run) {
	float delta = 1/60.f;
	MM::Engine engine(delta);

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();
	ASSERT_TRUE(provide_ret);

	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	// setup v system
	MM::AddSystemToScene(scene, MM::Systems::SimpleVelocity);

	auto e = scene.create();
	auto& t = scene.emplace<MM::Components::Transform2D>(e);
	auto& v = scene.emplace<MM::Components::Velocity2D>(e);
	t.position = { 0.f, 0.f };
	t.rotation = 0.f;

	v.velocity = { 1.f, 1.f };
	v.rotation = 0.f;

	engine.fixedUpdate();

	ASSERT_EQ(t.position.x, 1.f * delta);
}

