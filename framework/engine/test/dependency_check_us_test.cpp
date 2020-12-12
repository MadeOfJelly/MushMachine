#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <stdexcept>
#include <mm/engine.hpp>
#include <mm/update_strategies/update_strategy.hpp>
#include <mm/update_strategies/dependency_check_decorater.hpp>
#include <mm/services/service.hpp>

using ::testing::Return;
using ::testing::_;

#include <entt/core/hashed_string.hpp>
//using namespace ::entt::literals;

class MockUpdateStrategy : public MM::UpdateStrategies::UpdateStrategy {
	public:
		MOCK_METHOD(
			bool,
			registerService,
			(const entt::id_type s_id, std::vector<MM::UpdateStrategies::UpdateCreationInfo>&& info),
			(override)
		);

		// protected:
		MOCK_METHOD(void, doUpdate, (MM::Engine& engine), (override));

		MOCK_METHOD(bool, enableService, (const entt::id_type s_id), (override));
		MOCK_METHOD(bool, disableService, (const entt::id_type s_id), (override));

		// public:
		MOCK_METHOD(bool, enable, (const MM::UpdateStrategies::update_key_t key), (override));
		MOCK_METHOD(bool, disable, (const MM::UpdateStrategies::update_key_t key), (override));

		MOCK_METHOD(bool, depend, (const MM::UpdateStrategies::update_key_t A, const MM::UpdateStrategies::update_key_t B), (override));

		MOCK_METHOD(void, addDefered, (std::function<void(MM::Engine&)> function), (override));
};

class MockService : public MM::Services::Service {
	public:
		const char* name(void) override { return "MockService"; }

		MOCK_METHOD(bool, enable, (MM::Engine& engine), (override));
		MOCK_METHOD(void, disable, (MM::Engine& engine), (override));

		MOCK_METHOD(std::vector<MM::UpdateStrategies::UpdateCreationInfo>, registerUpdates, (), (override));
};

TEST(dependency_check_update_strategy, decoration_mock) {
	auto dep = std::make_unique<MM::UpdateStrategies::DependencyCheckDecorator<MockUpdateStrategy>>();
	auto* mock = static_cast<MockUpdateStrategy*>(dep.get());

	EXPECT_CALL(*mock, registerService(_, _))
		.Times(1);
	EXPECT_CALL(*mock, enableService(_))
		.Times(1);
	EXPECT_CALL(*mock, disableService(_))
		.Times(1);

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
		MM::Engine engine(std::move(dep));

		engine.addService<TmpMockService>();

		ASSERT_TRUE(engine.enableService<TmpMockService>());
		engine.disableService<TmpMockService>();
	}
}

TEST(dependency_check_update_strategy, simple_loop) {
	auto dep = std::make_unique<MM::UpdateStrategies::DependencyCheckDecorator<MockUpdateStrategy>>();
	auto* mock = static_cast<MockUpdateStrategy*>(dep.get());

	EXPECT_CALL(*mock, registerService(_, _))
		.Times(1);
	EXPECT_CALL(*mock, enableService(_))
		.Times(1);
	//EXPECT_CALL(*mock, disableService(_))
		//.Times(1);
	EXPECT_CALL(*mock, depend(_, _))
		.Times(1); // the layer should catch the error before forwarding it

	class BonkersService : public MM::Services::Service {
		public:
			virtual ~BonkersService(void) {}

			const char* name(void) override { return "BonkersService"; }

			bool enable(MM::Engine& engine) override {
				engine.getUpdateStrategy().depend("BonkerA"_hs, "BonkerB"_hs);

				// maleformed: direct cycle
				engine.getUpdateStrategy().depend("BonkerB"_hs, "BonkerA"_hs);

				return true;
			}

			void disable(MM::Engine&) override {}

			std::vector<MM::UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override {
				return {
					MM::UpdateStrategies::UpdateCreationInfo{
						"BonkerA"_hs,
						"BonkerA",
						[](MM::Engine&) {}
					},
					MM::UpdateStrategies::UpdateCreationInfo{
						"BonkerB"_hs,
						"BonkerB",
						[](MM::Engine&) {}
					}
				};
			}
	};

	{
		MM::Engine engine(std::move(dep));

		engine.addService<BonkersService>();

		ASSERT_THROW(engine.enableService<BonkersService>(), std::logic_error);

		//engine.disableService<BonkersService>();
	}
}

