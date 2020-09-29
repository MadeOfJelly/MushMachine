#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/screen_director.hpp>

class TestService1 : public MM::Services::Service {
	public:
		const char* name(void) override { return "TestService1"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}
};

class TestService2 : public MM::Services::Service {
	public:
		const char* name(void) override { return "TestService2"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}
};

class TestServiceInterface : public MM::Services::Service {
	public:
		const char* name(void) override { return "TestServiceInterface"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}

	public:
		virtual int test_method(void) = 0;
};

class TestServiceInterfaceImpl1 : public TestServiceInterface {
	public:
		const char* name(void) override { return "TestServiceInterfaceImpl1"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}

	public:
		int test_method(void) override {
			return 1;
		}
};

class TestServiceInterfaceImpl2 : public TestServiceInterface {
	public:
		const char* name(void) override { return "TestServiceInterfaceImpl2"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}

	public:
		int test_method(void) override {
			return 2;
		}
};

TEST(SceneDirectorSimple, offline) {
	MM::Engine engine;

	engine.addService<TestService1>();
	engine.addService<TestService2>();

	auto& sd = engine.addService<MM::Services::ScreenDirector>();

	// setup scene director
	ASSERT_EQ(sd.screens.size(), 0);

	// id 0 - start point, not required
	{
		auto& screen_0 = sd.screens["screen_0"];
		ASSERT_TRUE(screen_0.start_disable.empty()); // none
		ASSERT_TRUE(screen_0.start_enable.empty()); // none
		ASSERT_TRUE(screen_0.end_disable.empty()); // none
		ASSERT_TRUE(screen_0.end_enable.empty()); // none
	}

	// id 1
	{
		auto& screen_1 = sd.screens["screen_1"];
		screen_1.start_disable.push_back(engine.type<TestService2>());
		screen_1.start_enable.push_back(engine.type<TestService1>());
		screen_1.start_fn = [](auto&) {
			std::cout << "now in screen_1\n";
		};
		screen_1.end_fn = [](auto&) {
			std::cout << "exiting screen_1\n";
		};
	}

	// id 2
	{
		auto& screen_2 = sd.screens["screen_2"];
		screen_2.start_disable.push_back(engine.type<TestService1>());
		screen_2.start_enable.push_back(engine.type<TestService2>());
		screen_2.start_fn = [](auto&) {
			std::cout << "now in screen_2\n";
		};
		screen_2.end_fn = [](auto&) {
			std::cout << "exiting screen_2\n";
		};
	}

	sd.queued_screen_id = "screen_0"; // set to screen_0

	ASSERT_TRUE(engine.enableService<MM::Services::ScreenDirector>());

	sd.changeScreenTo(engine, "screen_1"); ASSERT_EQ(sd.curr_screen_id, "screen_1");
	sd.changeScreenTo(engine, "screen_2"); ASSERT_EQ(sd.curr_screen_id, "screen_2");
	sd.changeScreenTo(engine, "screen_1"); ASSERT_EQ(sd.curr_screen_id, "screen_1");
	sd.changeScreenTo(engine, "screen_0"); ASSERT_EQ(sd.curr_screen_id, "screen_0");
	sd.changeScreenTo(engine, "somthing which does not exits"); ASSERT_EQ(sd.curr_screen_id, "screen_0"); // does not exist, does nothing
}

TEST(SceneDirectorSimple, online) {
	MM::Engine engine;

	engine.addService<TestService1>();
	engine.addService<TestService2>();

	auto& sd = engine.addService<MM::Services::ScreenDirector>();

	// setup scene director
	ASSERT_EQ(sd.screens.size(), 0);

	// id 0 - start point
	{
		auto& screen_0 = sd.screens["screen_0"];
		//screen_0.start_disable.empty(); // none
		//screen_0.start_enable.empty(); // none
		ASSERT_TRUE(screen_0.start_disable.empty()); // none
		ASSERT_TRUE(screen_0.start_enable.empty()); // none
		ASSERT_TRUE(screen_0.end_disable.empty()); // none
		ASSERT_TRUE(screen_0.end_enable.empty()); // none
	}

	// id 1
	{
		auto& screen_1 = sd.screens["screen_1"];
		screen_1.start_disable.push_back(engine.type<TestService2>());
		screen_1.start_enable.push_back(engine.type<TestService1>());
		screen_1.start_fn = [](MM::Engine& engine) {
			std::cout << "now in screen_1\n";

			// and queue next screen immediately (for the purpose of this test)
			engine.tryService<MM::Services::ScreenDirector>()->queueChangeScreenTo("screen_2");
		};
		screen_1.end_fn = [](auto&) {
			std::cout << "exiting screen_1\n";
		};
	}

	// id 2
	{
		auto& screen_2 = sd.screens["screen_2"];
		screen_2.start_disable.push_back(engine.type<TestService1>());
		screen_2.start_enable.push_back(engine.type<TestService2>());
		screen_2.start_fn = [](MM::Engine& engine) {
			std::cout << "now in screen_2\n";

			// and queue next screen immediately (for the purpose of this test)
			engine.tryService<MM::Services::ScreenDirector>()->queueChangeScreenTo("screen_end");
		};
		screen_2.end_fn = [](auto&) {
			std::cout << "exiting screen_2\n";
		};
	}

	// id end, quits the engine
	{
		auto& screen_end = sd.screens["screen_end"];
		screen_end.start_fn = [](MM::Engine& engine) {
			engine.stop();
		};
	}

	sd.queued_screen_id = "screen_0"; // init

	ASSERT_TRUE(engine.enableService<MM::Services::ScreenDirector>());

	// TODO: remove screen 0
	sd.queueChangeScreenTo("screen_1"); // start "chain" (bc screen_0 does nothing), this is the same as an assigment

	engine.run();
}

TEST(SceneDirectorSimple, offline_provide) {
	MM::Engine engine;

	engine.addService<TestServiceInterfaceImpl1>();
	engine.addService<TestServiceInterfaceImpl2>();

	auto& sd = engine.addService<MM::Services::ScreenDirector>();

	// setup scene director
	ASSERT_EQ(sd.screens.size(), 0);

	// id 0 - start point, not required
	{
		auto& screen_0 = sd.screens["screen_0"];
		ASSERT_TRUE(screen_0.start_disable.empty()); // none
		ASSERT_TRUE(screen_0.start_enable.empty()); // none
		ASSERT_TRUE(screen_0.end_disable.empty()); // none
		ASSERT_TRUE(screen_0.end_enable.empty()); // none
	}

	// id 1
	{
		auto& screen_1 = sd.screens["screen_1"];
		screen_1.start_disable.push_back(engine.type<TestServiceInterface>()); // if already provided for
		screen_1.start_enable.push_back(engine.type<TestServiceInterfaceImpl1>());
		screen_1.start_provide.emplace_back(
			engine.type<TestServiceInterface>(),
			engine.type<TestServiceInterfaceImpl1>()
		);
		screen_1.start_fn = [](auto&) {
			std::cout << "now in screen_1\n";
			std::cout << "impl1 is provided\n";
		};
		screen_1.end_fn = [](auto&) {
			std::cout << "exiting screen_1\n";
		};
	}

	// id 2
	{
		auto& screen_2 = sd.screens["screen_2"];
		screen_2.start_disable.push_back(engine.type<TestServiceInterface>()); // if already provided for
		screen_2.start_enable.push_back(engine.type<TestServiceInterfaceImpl2>());
		screen_2.start_provide.emplace_back(
			engine.type<TestServiceInterface>(),
			engine.type<TestServiceInterfaceImpl2>()
		);
		screen_2.start_fn = [](auto&) {
			std::cout << "now in screen_2\n";
			std::cout << "impl2 is provided\n";
		};
		screen_2.end_fn = [](auto&) {
			std::cout << "exiting screen_2\n";
		};
	}

	sd.queued_screen_id = "screen_0"; // set to screen_0

	ASSERT_TRUE(engine.enableService<MM::Services::ScreenDirector>());
	// now in screen_0


	ASSERT_NE(engine.tryService<TestServiceInterfaceImpl1>(), nullptr);
	ASSERT_NE(engine.tryService<TestServiceInterfaceImpl2>(), nullptr);


	sd.changeScreenTo(engine, "screen_1"); ASSERT_EQ(sd.curr_screen_id, "screen_1");
	ASSERT_NE(engine.tryService<TestServiceInterface>(), nullptr);
	ASSERT_EQ(engine.tryService<TestServiceInterface>()->test_method(), 1);

	sd.changeScreenTo(engine, "screen_2"); ASSERT_EQ(sd.curr_screen_id, "screen_2");
	ASSERT_NE(engine.tryService<TestServiceInterface>(), nullptr);
	ASSERT_EQ(engine.tryService<TestServiceInterface>()->test_method(), 2);

	sd.changeScreenTo(engine, "screen_1"); ASSERT_EQ(sd.curr_screen_id, "screen_1");
	ASSERT_NE(engine.tryService<TestServiceInterface>(), nullptr);
	ASSERT_EQ(engine.tryService<TestServiceInterface>()->test_method(), 1);

	sd.changeScreenTo(engine, "screen_0"); ASSERT_EQ(sd.curr_screen_id, "screen_0");

	sd.changeScreenTo(engine, "somthing which does not exits"); ASSERT_EQ(sd.curr_screen_id, "screen_0"); // does not exist, does nothing
}

