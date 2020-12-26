#include <gtest/gtest.h>

#include <mm/engine_fwd.hpp>

#include <entt/entity/registry.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>

TEST(simple_velocity_2d, basic_run) {
	MM::Scene scene;

	// setup v system
	MM::AddSystemToScene(scene, MM::Systems::SimpleVelocity);

	auto e = scene.create();
	auto& t = scene.emplace<MM::Components::Transform2D>(e);
	auto& v = scene.emplace<MM::Components::Velocity2D>(e);
	t.position = { 0.f, 0.f };
	t.rotation = 0.f;

	v.velocity = { 1.f, 1.f };
	v.rotation = 0.f;

	::MM::EachSystemInScene(scene, [&](::MM::Scene& s, ::MM::System& fn) {
		fn(s, 1.f/60.f);
	});

	ASSERT_EQ(t.position.x, 1.f * 1.f/60.f);
}

