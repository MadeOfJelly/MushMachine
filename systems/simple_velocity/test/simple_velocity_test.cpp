#include <gtest/gtest.h>

#include <mm/engine_fwd.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>

TEST(simple_velocity_2d, basic_run) {
	MM::Scene scene;

	// setup v system
	auto& org = scene.set<entt::organizer>();
	org.emplace<&MM::Systems::simple_velocity>("simple_velocity");
	auto graph = org.graph();

	// setup delta
	auto& time_ctx = scene.ctx_or_set<MM::Components::TimeDelta>(1.f/60.f, 1.f);
	time_ctx.tickDelta = 1.f/60.f * time_ctx.deltaFactor;

	// setup test entity
	auto e = scene.create();
	auto& t = scene.emplace<MM::Components::Transform2D>(e);
	auto& v = scene.emplace<MM::Components::Velocity2D>(e);
	t.position = { 0.f, 0.f };
	t.rotation = 0.f;

	v.velocity = { 1.f, 1.f };
	v.rotation = 0.f;

	// run all systems
	for (auto&& vert : graph) {
		vert.callback()(vert.data(), scene);
	}

	ASSERT_EQ(t.position.x, 1.f * 1.f/60.f);
}

