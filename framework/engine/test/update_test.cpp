#include <gtest/gtest.h>

#include <mm/engine.hpp>

TEST(engine_fixed_update, empty_add_rm) {
	MM::Engine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.addFixedUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	handle.lock()->priority = 1;

	engine.removeFixedUpdate(handle);
}

TEST(engine_update, empty_add_rm) {
	MM::Engine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.addUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	handle.lock()->priority = 1;

	engine.removeUpdate(handle);
}

TEST(engine_fixed_update, empty_run) {
	MM::Engine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.addFixedUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	handle.lock()->priority = 1;

	engine.fixedUpdate(); // single update

	engine.removeFixedUpdate(handle);
}

TEST(engine_update, empty_run) {
	MM::Engine engine;

	auto test_fun = [](auto&) {};

	auto handle = engine.addUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	handle.lock()->priority = 1;

	engine.update();

	engine.removeUpdate(handle);
}

TEST(engine_fixed_update, test_run) {
	MM::Engine engine;

	bool run = false;

	auto test_fun = [&run](auto&) { run = true; };

	auto handle = engine.addFixedUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	handle.lock()->priority = 1;

	ASSERT_FALSE(run);
	engine.fixedUpdate(); // single update
	ASSERT_TRUE(run);

	engine.removeFixedUpdate(handle);
}

TEST(engine_update, test_run) {
	MM::Engine engine;

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

