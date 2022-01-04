#include <gtest/gtest.h>

#include <mm/engine_fwd.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>

TEST(simple_velocity_2d, basic_run) {
	MM::Scene scene;

	// setup v system
	auto& org = scene.set<entt::organizer>();
	org.emplace<&MM::Systems::simple_positional_velocity>("simple_positional_velocity");
	org.emplace<&MM::Systems::simple_rotational_velocity>("simple_rotational_velocity");
	auto graph = org.graph();

	// setup delta
	auto& time_ctx = scene.ctx_or_set<MM::Components::TimeDelta>(1.f/60.f, 1.f);
	time_ctx.tickDelta = 1.f/60.f * time_ctx.deltaFactor;

	// setup test entity
	auto e = scene.create();
	auto& p = scene.emplace<MM::Components::Position2D>(e);
	auto& r = scene.emplace<MM::Components::Rotation2D>(e);
	auto& vp = scene.emplace<MM::Components::Velocity2DPosition>(e);
	auto& vr = scene.emplace<MM::Components::Velocity2DRotation>(e);
	p.pos = { 0.f, 0.f };
	r.rot = 0.f;

	vp.pos_vel = { 1.f, 1.f };
	vr.rot_vel = 0.f;

	// run all systems
	for (auto&& vert : graph) {
		vert.callback()(vert.data(), scene);
	}

	ASSERT_EQ(p.pos.x, 1.f * 1.f/60.f);
}

