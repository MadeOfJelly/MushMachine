#include <gtest/gtest.h>

#include <mm/resource_manager.hpp>

#include <entt/resource/cache.hpp>

using namespace entt::literals; // :D

// disable, this just tests entt and not MM
#if 0
TEST(EngineResourceManagerTest, entt_cache) {
	struct type_entt_cache_test {
		int val;
	};
	entt::resource_cache<type_entt_cache_test> cache{};

	ASSERT_TRUE(cache.empty());

	struct type_entt_cache_test_loader final : entt::resource_loader<type_entt_cache_test_loader, type_entt_cache_test> {
		std::shared_ptr<type_entt_cache_test> load(int value) const {
			return std::shared_ptr<type_entt_cache_test>(new type_entt_cache_test{value});
		}
	};

	ASSERT_TRUE(cache.load<type_entt_cache_test_loader>("test1"_hs, 1));
	ASSERT_EQ(cache.size(), 1);
	auto h_test1 =  cache.handle("test1"_hs);
	ASSERT_TRUE(static_cast<bool>(h_test1));
	//ASSERT_EQ(*h_test1, 1);
}
#endif

TEST(EngineResourceManagerTest, basic) {
	struct type_test {
		int val;
	};
	auto& rm = MM::ResourceManager<type_test>::ref();

	ASSERT_TRUE(rm.empty());

	struct type_test_loader final {
		std::shared_ptr<type_test> load(int value) const {
			return std::shared_ptr<type_test>(new type_test{value});
		}
	};

	ASSERT_TRUE(rm.load<type_test_loader>("test1", 1));
	ASSERT_FALSE(rm.empty());
	ASSERT_EQ(rm.size(), 1);
	ASSERT_TRUE(rm.contains("test1"_hs));

	{
		auto h_test1 =  rm.get("test1"_hs);
		ASSERT_TRUE(static_cast<bool>(h_test1));
		ASSERT_EQ(h_test1->val, 1);

		ASSERT_TRUE(rm.reload<type_test_loader>("test1", 3));
		ASSERT_EQ(rm.size(), 1);
		ASSERT_TRUE(static_cast<bool>(h_test1));
		auto h_test1_2 =  rm.get("test1"_hs);
		ASSERT_TRUE(static_cast<bool>(h_test1_2));
		ASSERT_NE(h_test1->val, h_test1_2->val);

		{
			auto rt = rm.id_from_handle(h_test1_2);
			ASSERT_TRUE(static_cast<bool>(rt));
			ASSERT_EQ(rt, "test1"_hs);
		}


		rm.discard("test1");
		ASSERT_TRUE(rm.empty());

		{
			auto rt = rm.id_from_handle(h_test1_2);
			ASSERT_FALSE(static_cast<bool>(rt));
		}
	}
}

