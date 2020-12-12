#include <gtest/gtest.h>

#include <memory>
#include <mm/engine.hpp>

//class MyUpdateStrategy : public MM::UpdateStrategy::UpdateStrategy {
	//public:
		//virtual ~MyUpdateStrategy(void) {}

		//// return nullptr on error
		//[[nodiscard]] MM::UpdateStrategy::FunctionDataHandle addUpdate(std::function<void(MM::Engine&)> fn) override {
		//}

		//[[nodiscard]] MM::UpdateStrategy::FunctionDataHandle addFixedUpdate(std::function<void(MM::Engine&)> fn) override {
		//}

		//// knows which one
		//void removeUpdate(MM::UpdateStrategy::FunctionDataHandle handle) override {
		//}

		//// dont use, if you are not using it to modify the engine.
		//// you usualy dont need to use this, if you think you need to use this, you probably dont.
		//void addFixedDefered(std::function<void(MM::Engine&)> function) override {
		//}

		////virtual std::future addAsync(std::function<void(Engine&)> function) = 0;

		//void doUpdate(MM::Engine& engine) override {
		//}

		//void doFixedUpdate(MM::Engine& engine) override {
		//}

//};

class MyEngine : public MM::Engine {
	public:
		MyEngine(void) : MM::Engine(std::make_unique<MyUpdateStrategy>()) {
		}
};

TEST(engine_fixed_update, empty_add_rm) {
	MyEngine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.getUpdateStrategy().addFixedUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	//handle.lock()->priority = 1;

	engine.getUpdateStrategy().removeUpdate(handle);
}

TEST(engine_update, empty_add_rm) {
	MyEngine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.getUpdateStrategy().addUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	//handle.lock()->priority = 1;

	engine.getUpdateStrategy().removeUpdate(handle);
}

TEST(engine_fixed_update, empty_run) {
	MyEngine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.getUpdateStrategy().addFixedUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	//handle.lock()->priority = 1;

	engine.fixedUpdate(); // single update

	engine.getUpdateStrategy().removeUpdate(handle);
}

TEST(engine_update, empty_run) {
	MyEngine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.getUpdateStrategy().addUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	//handle.lock()->priority = 1;

	engine.update();

	engine.getUpdateStrategy().removeUpdate(handle);
}

TEST(engine_fixed_update, test_run) {
	MyEngine engine;

	bool run = false;

	auto test_fun = [&run](auto&) { run = true; };

	auto handle = engine.getUpdateStrategy().addFixedUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	//handle.lock()->priority = 1;

	ASSERT_FALSE(run);
	engine.fixedUpdate(); // single update
	ASSERT_TRUE(run);

	engine.getUpdateStrategy().removeUpdate(handle);
}

#if 0

TEST(engine_update, test_run) {
	MyEngine engine;

	bool run = false;

	auto test_fun = [&run](auto&) { run = true; };

	auto handle = engine.addUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	handle.lock()->priority = 1;

	ASSERT_FALSE(run);
	engine.update();
	ASSERT_TRUE(run);

	engine.removeUpdate(handle);
}

TEST(engine_fixed_update, test_order_run) {
	MyEngine engine;

	bool run1 = false;
	bool run2 = false;

	auto test_fun1 = [&](auto&) {
		ASSERT_FALSE(run2);
		run1 = true;
	};
	auto test_fun2 = [&](auto&) {
		ASSERT_TRUE(run1);
		run2 = true;
	};

	auto handle1 = engine.addFixedUpdate(test_fun1);
	ASSERT_NE(handle1.lock(), nullptr);
	handle1.lock()->priority = 1;

	auto handle2 = engine.addFixedUpdate(test_fun2);
	ASSERT_NE(handle2.lock(), nullptr);
	handle2.lock()->priority = 0;

	ASSERT_FALSE(run1);
	ASSERT_FALSE(run2);
	engine.fixedUpdate(); // single update
	ASSERT_TRUE(run1);
	ASSERT_TRUE(run2);

	engine.removeFixedUpdate(handle1);
	engine.removeFixedUpdate(handle2);
}

TEST(engine_fixed_update, test_order_rev_run) {
	MM::Engine engine;

	bool run1 = false;
	bool run2 = false;

	auto test_fun1 = [&](auto&) {
		ASSERT_TRUE(run2);
		run1 = true;
	};
	auto test_fun2 = [&](auto&) {
		ASSERT_FALSE(run1);
		run2 = true;
	};

	auto handle1 = engine.addFixedUpdate(test_fun1);
	ASSERT_NE(handle1.lock(), nullptr);
	handle1.lock()->priority = 0;

	auto handle2 = engine.addFixedUpdate(test_fun2);
	ASSERT_NE(handle2.lock(), nullptr);
	handle2.lock()->priority = 1;

	ASSERT_FALSE(run1);
	ASSERT_FALSE(run2);
	engine.fixedUpdate(); // single update
	ASSERT_TRUE(run1);
	ASSERT_TRUE(run2);

	engine.removeFixedUpdate(handle1);
	engine.removeFixedUpdate(handle2);
}

TEST(engine_update, test_order_run) {
	MM::Engine engine;

	bool run1 = false;
	bool run2 = false;

	auto test_fun1 = [&](auto&) {
		ASSERT_FALSE(run2);
		run1 = true;
	};
	auto test_fun2 = [&](auto&) {
		ASSERT_TRUE(run1);
		run2 = true;
	};

	auto handle1 = engine.addUpdate(test_fun1);
	ASSERT_NE(handle1.lock(), nullptr);
	handle1.lock()->priority = 1;

	auto handle2 = engine.addUpdate(test_fun2);
	ASSERT_NE(handle2.lock(), nullptr);
	handle2.lock()->priority = 0;

	ASSERT_FALSE(run1);
	ASSERT_FALSE(run2);
	engine.update(); // single update
	ASSERT_TRUE(run1);
	ASSERT_TRUE(run2);

	engine.removeUpdate(handle1);
	engine.removeUpdate(handle2);
}

TEST(engine_update, test_order_rev_run) {
	MM::Engine engine;

	bool run1 = false;
	bool run2 = false;

	auto test_fun1 = [&](auto&) {
		ASSERT_TRUE(run2);
		run1 = true;
	};
	auto test_fun2 = [&](auto&) {
		ASSERT_FALSE(run1);
		run2 = true;
	};

	auto handle1 = engine.addUpdate(test_fun1);
	ASSERT_NE(handle1.lock(), nullptr);
	handle1.lock()->priority = 0;

	auto handle2 = engine.addUpdate(test_fun2);
	ASSERT_NE(handle2.lock(), nullptr);
	handle2.lock()->priority = 1;

	ASSERT_FALSE(run1);
	ASSERT_FALSE(run2);
	engine.update(); // single update
	ASSERT_TRUE(run1);
	ASSERT_TRUE(run2);

	engine.removeUpdate(handle1);
	engine.removeUpdate(handle2);
}

#endif
