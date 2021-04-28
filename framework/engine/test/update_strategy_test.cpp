#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <mm/engine.hpp>
#include <mm/update_strategies/update_strategy.hpp>

#include <entt/core/hashed_string.hpp>

using ::testing::Return;
using ::testing::_;

using namespace entt::literals;

class MockUpdateStrategy : public MM::UpdateStrategies::UpdateStrategy {
	public:
		const char* name(void) override { return "MockUpdateStrategy"; }

		// protected:
		MOCK_METHOD(void, doUpdate, (MM::Engine& engine), (override));

		MOCK_METHOD(bool, enableService, (const entt::id_type s_id, std::vector<MM::UpdateStrategies::TaskInfo>&& task_array), (override));
		MOCK_METHOD(bool, disableService, (const entt::id_type s_id), (override));

		// public:
		MOCK_METHOD(void, addDeferred, (std::function<void(MM::Engine&)> function), (override));
		MOCK_METHOD(void, addAsync, (std::function<void(MM::Engine&)> function), (override));

		MOCK_METHOD(void, forEachTask, (std::function<bool(MM::UpdateStrategies::TaskInfo&)> function), (override));
};

class MockService : public MM::Services::Service {
	public:
		const char* name(void) override { return "MockService"; }

		MOCK_METHOD(bool, enable, (MM::Engine& engine, std::vector<MM::UpdateStrategies::TaskInfo>& task_array), (override));
		MOCK_METHOD(void, disable, (MM::Engine& engine), (override));
};

TEST(engine_mock, update_strategy_run) {
	auto mock = std::make_unique<MockUpdateStrategy>();

	EXPECT_CALL(*mock, doUpdate(_))
		.Times(1);

	MM::Engine engine(std::move(mock));

	engine.update();
}

TEST(engine_mock, service_update_strategy) {
	auto mock = std::make_unique<MockUpdateStrategy>();

	EXPECT_CALL(*mock, enableService(_, _))
		.Times(1);
	EXPECT_CALL(*mock, disableService(_))
		.Times(1);

	class TmpMockService : public MockService {
		public:
			TmpMockService(void) {
				EXPECT_CALL(*this, enable(_, _))
					.Times(1);
				ON_CALL(*this, enable(_, _))
					.WillByDefault(Return(true));

				EXPECT_CALL(*this, disable(_))
					.Times(1);
			}
	};

	{
		MM::Engine engine(std::move(mock));

		engine.addService<TmpMockService>();

		ASSERT_TRUE(engine.enableService<TmpMockService>());
		engine.disableService<TmpMockService>();
	}
}

TEST(engine_mock, service_update_strategy_run_1) {
	auto mock = std::make_unique<MockUpdateStrategy>();

	EXPECT_CALL(*mock, enableService(_, _))
		.Times(1);
	EXPECT_CALL(*mock, disableService(_))
		.Times(1);

	class TmpMockService : public MockService {
		public:
			explicit TmpMockService(void) {
				EXPECT_CALL(*this, enable)
					.Times(1);
				ON_CALL(*this, enable)
					.WillByDefault([](MM::Engine&, std::vector<MM::UpdateStrategies::TaskInfo>& task_array) -> bool {
						using MM::UpdateStrategies::TaskInfo;

						task_array.push_back(
							TaskInfo{"TmpMockService"}
								//.precede("PreviousTask")
								.fn([](MM::Engine& engine) { (void)engine; })
						);

						return true;
					});

				EXPECT_CALL(*this, disable(_))
					.Times(1);
			}
	};

	{
		MM::Engine engine(std::move(mock));

		engine.addService<TmpMockService>();

		ASSERT_TRUE(engine.enableService<TmpMockService>());
		engine.disableService<TmpMockService>();
	}
}

