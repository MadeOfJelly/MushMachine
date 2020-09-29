#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/simple_scene.hpp>
#include <entt/entity/registry.hpp>

TEST(simple_scene, add_en_dis) {
	MM::Engine e;

	e.addService<MM::Services::SimpleSceneService>();

	ASSERT_TRUE(e.enableService<MM::Services::SimpleSceneService>());

	{
		auto* sss = e.tryService<MM::Services::SimpleSceneService>();
		ASSERT_NE(sss, nullptr);

		auto& s = sss->getScene();
		auto e = s.create();
		ASSERT_TRUE(s.valid(e));
	}

	e.disableService<MM::Services::SimpleSceneService>();
}

TEST(simple_scene, add_en_dis_provide) {
	MM::Engine e;

	e.addService<MM::Services::SimpleSceneService>();
	e.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();

	ASSERT_TRUE(e.enableService<MM::Services::SimpleSceneService>());

	{
		auto* ssi = e.tryService<MM::Services::SceneServiceInterface>();
		ASSERT_NE(ssi, nullptr);

		auto& s = ssi->getScene();
		auto e = s.create();
		ASSERT_TRUE(s.valid(e));
	}

	e.disableService<MM::Services::SimpleSceneService>();
}

TEST(simple_scene, change_scene) {
	MM::Engine engine;

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	{
		auto* sss = engine.tryService<MM::Services::SimpleSceneService>();
		ASSERT_NE(sss, nullptr);

		auto e = sss->getScene().create();
		ASSERT_TRUE(sss->getScene().valid(e));

		sss->changeScene(std::make_unique<MM::Scene>());

		ASSERT_TRUE(sss->getScene().valid(e));

		engine.fixedUpdate();

		ASSERT_FALSE(sss->getScene().valid(e));
	}

	engine.disableService<MM::Services::SimpleSceneService>();
}

