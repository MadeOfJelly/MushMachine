#include "mm/update_strategies/update_strategy.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <mm/engine.hpp>
#include <mm/update_strategies/default_strategy.hpp>

#include <entt/core/hashed_string.hpp>

using ::testing::Return;
using ::testing::_;

using namespace entt::literals;

class MockService : public MM::Services::Service {
	public:
		const char* name(void) override { return "MockService"; }

		MOCK_METHOD(bool, enable, (MM::Engine& engine), (override));
		MOCK_METHOD(void, disable, (MM::Engine& engine), (override));

		MOCK_METHOD(std::vector<MM::UpdateStrategies::UpdateCreationInfo>, registerUpdates, (), (override));
};

TEST(default_update_strategy, simple_update) {
	MM::Engine engine(std::make_unique<MM::UpdateStrategies::SingleThreadedDefault>());

	engine.update();
}

// note: we test pure single-threaded behaviour, so nothing needs to be thread save
TEST(default_update_strategy, async) {
	int counter = 1;
	MM::Engine engine(std::make_unique<MM::UpdateStrategies::SingleThreadedDefault>());
	ASSERT_EQ(counter, 1);

	engine.getUpdateStrategy().addAsync([&counter](MM::Engine&) { counter++; });

	engine.update();

	ASSERT_EQ(counter, 2);

	for (size_t i = 0; i < 100; i++) {
		engine.getUpdateStrategy().addAsync([&counter](MM::Engine&) { counter++; });
	}

	// make sure we dont have an infinity loop in case something is wrong
	size_t max_loop_safety_count = 0;

	do {
		engine.update();
		max_loop_safety_count++;
	} while (counter != 102 && max_loop_safety_count < 10000);

	ASSERT_EQ(counter, 102);
}

TEST(default_update_strategy, service) {
	class TmpMockService : public MockService {
		public:
			TmpMockService(void) {
				EXPECT_CALL(*this, registerUpdates())
					.Times(1);

				EXPECT_CALL(*this, enable(_))
					.Times(1);
				ON_CALL(*this, enable(_))
					.WillByDefault(Return(true));

				EXPECT_CALL(*this, disable(_))
					.Times(1);
			}
	};

	{
		MM::Engine engine(std::make_unique<MM::UpdateStrategies::SingleThreadedDefault>());

		engine.addService<TmpMockService>();

		ASSERT_TRUE(engine.enableService<TmpMockService>());
		engine.disableService<TmpMockService>();
	}
}

TEST(default_update_strategy, run_1) {
	class TmpMockService : public MockService {
		int& _counter;
		public:
			explicit TmpMockService(int& counter) : _counter(counter) {
				EXPECT_CALL(*this, registerUpdates())
					.Times(1);
				ON_CALL(*this, registerUpdates())
					.WillByDefault([this]() -> std::vector<MM::UpdateStrategies::UpdateCreationInfo> {
						return {
							{
								"TmpMockService"_hs,
								"TmpMockService",
								[this](MM::Engine&) { _counter++; }
							}
						};
					});

				EXPECT_CALL(*this, enable(_))
					.Times(1);
				ON_CALL(*this, enable(_))
					.WillByDefault(Return(true));

				EXPECT_CALL(*this, disable(_))
					.Times(1);
			}
	};

	{
		int counter = 1;
		MM::Engine engine(std::make_unique<MM::UpdateStrategies::SingleThreadedDefault>());
		ASSERT_EQ(counter, 1);

		engine.addService<TmpMockService>(counter);
		ASSERT_EQ(counter, 1);

		ASSERT_TRUE(engine.enableService<TmpMockService>());
		ASSERT_EQ(counter, 1);

		engine.getUpdateStrategy().addDeferred([](MM::Engine& e) { e.stop(); });

		engine.run();

		ASSERT_EQ(counter, 2);

		engine.disableService<TmpMockService>();

		ASSERT_EQ(counter, 2);
	}
}

