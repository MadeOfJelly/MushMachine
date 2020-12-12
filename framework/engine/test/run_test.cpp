#include <gtest/gtest.h>

#include <memory>
#include <mm/engine.hpp>

TEST(engine_run, test_run) {
	MM::Engine engine{std::make_unique};

	bool run = false;

	auto test_fun = [&run](auto& e) {
		run = true;
		e.stop();
	};

	auto handle = engine.addFixedUpdate(test_fun);
	ASSERT_NE(handle.lock(), nullptr);

	handle.lock()->priority = 1;

	ASSERT_FALSE(run);
	engine.run();
	ASSERT_TRUE(run);

	engine.removeFixedUpdate(handle);
}

TEST(engine_run, test_mult_run) {
	MM::Engine engine;

	bool run = false;
	unsigned int fu_count = 0;
	unsigned int u_count = 0;

	const unsigned int f_to_do = 4;

	auto test_f_fun = [&](auto& e) {
		run = true;
		fu_count++;

		if (fu_count >= f_to_do)
			e.stop();
	};

	auto handle_f = engine.addFixedUpdate(test_f_fun);
	ASSERT_NE(handle_f.lock(), nullptr);
	handle_f.lock()->priority = 1;

	auto handle = engine.addUpdate([&u_count](auto&) { u_count++; });
	ASSERT_NE(handle_f.lock(), nullptr);
	handle.lock()->priority = 1;

	ASSERT_FALSE(run);
	engine.run();
	ASSERT_TRUE(run);

	EXPECT_GT(u_count, f_to_do) << "expected more update runs than fixed update runs...";
	std::cout << "while performing " << f_to_do << " fixed updates, the engine did " << u_count << " updates.\n";

	engine.removeFixedUpdate(handle_f);
}

