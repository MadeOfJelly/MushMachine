#include <gtest/gtest.h>

#include <mm/scalar_range2.hpp>
#include <mm/serialize/json_scalar_range2.hpp>

#include <cstdint>

TEST(std_utils_scalar_range2_json, signed_integer) {
	MM::ScalarRange2<int8_t> r8 {-4, 100};

	nlohmann::json j = r8;
	ASSERT_EQ(j["min"], -4);
	ASSERT_EQ(j["max"], 100);

	MM::ScalarRange2<int8_t> r8_2 = j;
	ASSERT_EQ(r8_2.min(), -4);
	ASSERT_EQ(r8_2.max(), 100);
}

TEST(std_utils_scalar_range2_json, unsigned_integer) {
	MM::ScalarRange2<uint8_t> r8 {4, 100};

	nlohmann::json j = r8;
	ASSERT_EQ(j["min"], 4);
	ASSERT_EQ(j["max"], 100);

	MM::ScalarRange2<uint8_t> r8_2 = j;
	ASSERT_EQ(r8_2.min(), 4);
	ASSERT_EQ(r8_2.max(), 100);
}

TEST(std_utils_scalar_range2_json, floating) {
	MM::ScalarRange2<float> r {-4.3f, 100.f};

	nlohmann::json j = r;
	ASSERT_EQ(j["min"], -4.3f);
	ASSERT_EQ(j["max"], 100.f);

	MM::ScalarRange2<float> r_2 = j;
	ASSERT_EQ(r_2.min(), -4.3f);
	ASSERT_EQ(r_2.max(), 100.f);
}

