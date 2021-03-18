#include <gtest/gtest.h>

#include <mm/scalar_range2.hpp>

#include <cstdint>

TEST(std_utils_scalar_range2, signed_integer_good) {
	MM::ScalarRange2<int8_t> r8;
	ASSERT_EQ(r8.min(), 0) << "default initialized";
	ASSERT_EQ(r8.max(), 0) << "default initialized";

	MM::ScalarRange2<int16_t> r16 {};
	ASSERT_EQ(r16.min(), 0) << "default initialized";
	ASSERT_EQ(r16.max(), 0) << "default initialized";

	MM::ScalarRange2<int32_t> r32 {1, 10};
	ASSERT_EQ(r32.min(), 1) << "ctr";
	ASSERT_EQ(r32.max(), 10) << "ctr";

	MM::ScalarRange2<int64_t> r64{-100, 42};
	ASSERT_EQ(r64.min(), -100) << "ctr";
	ASSERT_EQ(r64.max(), 42) << "ctr";

	ASSERT_TRUE(r64.inRange(0));
	ASSERT_TRUE(r64.inRange(-100));
	ASSERT_TRUE(r64.inRange(42));
	ASSERT_FALSE(r64.inRange(-101));
	ASSERT_FALSE(r64.inRange(-100001));
	ASSERT_FALSE(r64.inRange(100001));
	ASSERT_FALSE(r64.inRange(43));

	r64.sanitize();
	ASSERT_EQ(r64.min(), -100) << "sanitize";
	ASSERT_EQ(r64.max(), 42) << "sanitize";

	MM::ScalarRange2<int64_t> r64_2{r64.min(), r64.max()};
	ASSERT_EQ(r64.min(), r64_2.min());
	ASSERT_EQ(r64.max(), r64_2.max());
	MM::ScalarRange2<int64_t> r64_3{0, 100}; // different range


	// eq
	ASSERT_EQ(r64, r64_2);
	ASSERT_NE(r64, r64_3);

	// cpy
	MM::ScalarRange2<int64_t> r64_cpy_ctr{r64};
	ASSERT_EQ(r64_cpy_ctr, r64_2);
	ASSERT_NE(r64_cpy_ctr, r64_3);

	MM::ScalarRange2<int64_t> r64_cpy_ass{};
	r64_cpy_ass = r64;
	ASSERT_EQ(r64_cpy_ass, r64_2);
	ASSERT_NE(r64_cpy_ass, r64_3);
}

TEST(std_utils_scalar_range2, signed_integer_evil) {
	{
		MM::ScalarRange2<int32_t> r32 {100, -10}; // min > max in ctr
		ASSERT_EQ(r32.min(), -10);
		ASSERT_EQ(r32.max(), 100);
	}

	{
		MM::ScalarRange2<int32_t> r32 {100, 100}; // min == max
		ASSERT_EQ(r32.min(), 100);
		ASSERT_EQ(r32.max(), 100);
	}

	{
		MM::ScalarRange2<int32_t> r32 {100, 100}; // min == max
		ASSERT_EQ(r32.min(), 100);
		ASSERT_EQ(r32.max(), 100);

		r32.v_max = 20; // max < min, manual edit
		ASSERT_EQ(r32.min(), 100);
		ASSERT_EQ(r32.max(), 20);

		ASSERT_FALSE(r32.inRange(100));
		ASSERT_FALSE(r32.inRange(-100));
		ASSERT_FALSE(r32.inRange(20));
		ASSERT_FALSE(r32.inRange(0));

		r32.sanitize();

		ASSERT_EQ(r32.min(), 20);
		ASSERT_EQ(r32.max(), 100);
	}
}

TEST(std_utils_scalar_range2, unsigned_integer_good) {
	MM::ScalarRange2<uint8_t> r8;
	ASSERT_EQ(r8.min(), 0) << "default initialized";
	ASSERT_EQ(r8.max(), 0) << "default initialized";

	MM::ScalarRange2<uint16_t> r16 {};
	ASSERT_EQ(r16.min(), 0) << "default initialized";
	ASSERT_EQ(r16.max(), 0) << "default initialized";

	MM::ScalarRange2<uint32_t> r32 {1, 10};
	ASSERT_EQ(r32.min(), 1) << "ctr";
	ASSERT_EQ(r32.max(), 10) << "ctr";

	MM::ScalarRange2<uint64_t> r64{1, 42};
	ASSERT_EQ(r64.min(), 1) << "ctr";
	ASSERT_EQ(r64.max(), 42) << "ctr";

	ASSERT_TRUE(r64.inRange(1));
	ASSERT_TRUE(r64.inRange(10));
	ASSERT_TRUE(r64.inRange(42));
	ASSERT_FALSE(r64.inRange(0));
	ASSERT_FALSE(r64.inRange(101));
	ASSERT_FALSE(r64.inRange(100001));
	ASSERT_FALSE(r64.inRange(100001));
	ASSERT_FALSE(r64.inRange(43));

	r64.sanitize();
	ASSERT_EQ(r64.min(), 1) << "sanitize";
	ASSERT_EQ(r64.max(), 42) << "sanitize";

	MM::ScalarRange2<uint64_t> r64_2{r64.min(), r64.max()};
	ASSERT_EQ(r64.min(), r64_2.min());
	ASSERT_EQ(r64.max(), r64_2.max());
	MM::ScalarRange2<uint64_t> r64_3{0, 100}; // different range


	// eq
	ASSERT_EQ(r64, r64_2);
	ASSERT_NE(r64, r64_3);

	// cpy
	MM::ScalarRange2<uint64_t> r64_cpy_ctr{r64};
	ASSERT_EQ(r64_cpy_ctr, r64_2);
	ASSERT_NE(r64_cpy_ctr, r64_3);

	MM::ScalarRange2<uint64_t> r64_cpy_ass{};
	r64_cpy_ass = r64;
	ASSERT_EQ(r64_cpy_ass, r64_2);
	ASSERT_NE(r64_cpy_ass, r64_3);
}

TEST(std_utils_scalar_range2, unsigned_integer_evil) {
	{
		MM::ScalarRange2<uint32_t> r32 {100, 10}; // min > max in ctr
		ASSERT_EQ(r32.min(), 10);
		ASSERT_EQ(r32.max(), 100);
	}

	{
		MM::ScalarRange2<uint32_t> r32 {100, 100}; // min == max
		ASSERT_EQ(r32.min(), 100);
		ASSERT_EQ(r32.max(), 100);
	}

	{
		MM::ScalarRange2<uint32_t> r32 {100, 100}; // min == max
		ASSERT_EQ(r32.min(), 100);
		ASSERT_EQ(r32.max(), 100);

		r32.v_max = 20; // max < min, manual edit
		ASSERT_EQ(r32.min(), 100);
		ASSERT_EQ(r32.max(), 20);

		ASSERT_FALSE(r32.inRange(100));
		ASSERT_FALSE(r32.inRange(20));
		ASSERT_FALSE(r32.inRange(0));

		r32.sanitize();

		ASSERT_EQ(r32.min(), 20);
		ASSERT_EQ(r32.max(), 100);
	}
}

TEST(std_utils_scalar_range2, floating_good) {
	{
		MM::ScalarRange2<float> r32;
		ASSERT_EQ(r32.min(), 0.f) << "default initialized";
		ASSERT_EQ(r32.max(), 0.f) << "default initialized";
	}

	{
		MM::ScalarRange2<float> r32 {};
		ASSERT_EQ(r32.min(), 0.f) << "default initialized";
		ASSERT_EQ(r32.max(), 0.f) << "default initialized";
	}

	{
		MM::ScalarRange2<float> r32 {1.f, 10.f};
		ASSERT_EQ(r32.min(), 1.f) << "ctr";
		ASSERT_EQ(r32.max(), 10.f) << "ctr";
	}

	MM::ScalarRange2<double> r64{1., 42.};
	ASSERT_EQ(r64.min(), 1.) << "ctr";
	ASSERT_EQ(r64.max(), 42.) << "ctr";

	ASSERT_TRUE(r64.inRange(1.));
	ASSERT_TRUE(r64.inRange(1.1));
	ASSERT_TRUE(r64.inRange(1.3));
	ASSERT_TRUE(r64.inRange(10.));
	ASSERT_TRUE(r64.inRange(42.));
	ASSERT_FALSE(r64.inRange(0.));
	ASSERT_FALSE(r64.inRange(101.));
	ASSERT_FALSE(r64.inRange(100001.));
	ASSERT_FALSE(r64.inRange(100001.));
	ASSERT_FALSE(r64.inRange(43.));

	r64.sanitize();
	ASSERT_EQ(r64.min(), 1.) << "sanitize";
	ASSERT_EQ(r64.max(), 42.) << "sanitize";

	MM::ScalarRange2<double> r64_2{r64.min(), r64.max()};
	ASSERT_EQ(r64.min(), r64_2.min());
	ASSERT_EQ(r64.max(), r64_2.max());
	MM::ScalarRange2<double> r64_3{0., 100.}; // different range


	// eq
	ASSERT_EQ(r64, r64_2);
	ASSERT_NE(r64, r64_3);

	// cpy
	MM::ScalarRange2<double> r64_cpy_ctr{r64};
	ASSERT_EQ(r64_cpy_ctr, r64_2);
	ASSERT_NE(r64_cpy_ctr, r64_3);

	MM::ScalarRange2<double> r64_cpy_ass{};
	r64_cpy_ass = r64;
	ASSERT_EQ(r64_cpy_ass, r64_2);
	ASSERT_NE(r64_cpy_ass, r64_3);
}

TEST(std_utils_scalar_range2, floating_evil) {
	{
		MM::ScalarRange2<float> r32 {100.f, -10.f}; // min > max in ctr
		ASSERT_EQ(r32.min(), -10.f);
		ASSERT_EQ(r32.max(), 100.f);
	}

	{
		MM::ScalarRange2<float> r32 {100.f, 100.f}; // min == max
		ASSERT_EQ(r32.min(), 100.f);
		ASSERT_EQ(r32.max(), 100.f);
	}

	{
		MM::ScalarRange2<float> r32 {100.f, 100.f}; // min == max
		ASSERT_EQ(r32.min(), 100.f);
		ASSERT_EQ(r32.max(), 100.f);

		r32.v_max = 20.f; // max < min, manual edit
		ASSERT_EQ(r32.min(), 100.f);
		ASSERT_EQ(r32.max(), 20.f);

		ASSERT_FALSE(r32.inRange(100.f));
		ASSERT_FALSE(r32.inRange(-100.f));
		ASSERT_FALSE(r32.inRange(20.f));
		ASSERT_FALSE(r32.inRange(0.f));

		r32.sanitize();

		ASSERT_EQ(r32.min(), 20.f);
		ASSERT_EQ(r32.max(), 100.f);
	}
}

