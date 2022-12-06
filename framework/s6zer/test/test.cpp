#include <mm/s6zer/serialize.hpp>

#include <array>
#include <ostream>

#include <mm/random/srng.hpp>
#include <mm/scalar_range2.hpp>

#include <gtest/gtest.h>

namespace MM {
	template<typename Char, typename T>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& out, const MM::ScalarRange2<T>& range) {
		return out << "{ min: " << static_cast<int64_t>(range.min()) << ", max: " << static_cast<int64_t>(range.max()) << " }";
	}
} // MM

TEST(s6zer, bits_required_static) {
	static_assert(MM::s6zer::bits_required(0)== 0);
	static_assert(MM::s6zer::bits_required(1)== 1);
	static_assert(MM::s6zer::bits_required(2)== 2);
	static_assert(MM::s6zer::bits_required(3)== 2);
	static_assert(MM::s6zer::bits_required(4)== 3);
	static_assert(MM::s6zer::bits_required(32)== 6);
	static_assert(MM::s6zer::bits_required(0xffffffff)== 32);
	static_assert(MM::s6zer::bits_required(0xffffff00)== 32);
	static_assert(MM::s6zer::bits_required(0xf0000a00)== 32);
	static_assert(MM::s6zer::bits_required(0x0f000000)== 28);
	static_assert(MM::s6zer::bits_required(0x0000f000)== 16);
}

TEST(s6zer, byte_swap) {
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint8_t>(0x00)) == 0x00);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint8_t>(0xff)) == 0xff);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint8_t>(0x10)) == 0x10);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint8_t>(0xfe)) == 0xfe);

	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint16_t>(0x0000)) == 0x0000);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint16_t>(0xffff)) == 0xffff);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint16_t>(0x00fe)) == 0xfe00);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint16_t>(0xfefe)) == 0xfefe);

	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint32_t>(0x00000000)) == 0x00000000);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint32_t>(0xffffffff)) == 0xffffffff);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint32_t>(0xf0f00000)) == 0x0000f0f0);
	static_assert(MM::s6zer::detail::byte_swap(static_cast<uint32_t>(0xfe0000ef)) == 0xef0000fe);
}

TEST(s6zer, stream_normalcase1) {
	const uint32_t num1_orig {0b111};
	const uint32_t num1_orig_bits {3};
	const uint32_t num2_orig {0b1111111111};
	const uint32_t num2_orig_bits {10};
	const uint32_t num3_orig {0b111111111111111111111111};
	const uint32_t num3_orig_bits {24};

	std::array<uint32_t, 2> buffer;
	size_t buffer_size = buffer.size()*sizeof(uint32_t);
	{
		MM::s6zer::StreamWriter writer{buffer.data(), buffer_size};

		bool r = false;
		ASSERT_EQ(writer._scratch, 0x0);
		ASSERT_EQ(writer._scratch_bits, 0);
		ASSERT_EQ(writer._word_index, 0);
		ASSERT_EQ(writer._bits_written, 0);
		ASSERT_EQ(writer.bytesWritten(), 0);

		r = writer.serializeBits(num1_orig, num1_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0b0000000000000000000000000000000000000000000000000000000000000'111);
		ASSERT_EQ(writer._scratch_bits, 3);
		ASSERT_EQ(writer._word_index, 0);
		ASSERT_EQ(writer._bits_written, num1_orig_bits);

		r = writer.serializeBits(num2_orig, num2_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0b000000000000000000000000000000000000000000000000000'111'1111111111);
		ASSERT_EQ(writer._scratch_bits, 3+10);
		ASSERT_EQ(writer._word_index, 0);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits);

		r = writer.serializeBits(num3_orig, num3_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0b00000000000000000000000000000000000000000000000000000000000'11111);
		ASSERT_EQ(writer._scratch_bits, (3+10+24)-32);
		ASSERT_EQ(writer._word_index, 1);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits+num3_orig_bits);

		r = writer.flush();
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0);
		ASSERT_EQ(writer._scratch_bits, 0);
		ASSERT_EQ(writer._word_index, 2);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits+num3_orig_bits); // flush does not change bits written
		ASSERT_EQ(writer.bytesWritten(), 5); // 4.625 , so ceil

		buffer_size = writer.bytesWritten();
	}

	std::cout << "buffer_size: " << buffer_size << "\n";

	ASSERT_EQ(buffer[0], 0xffffffff);
	ASSERT_EQ(buffer[1], 0b000000000000000000000000000'11111);

	{
		MM::s6zer::StreamReader reader{buffer.data(), buffer_size};

		bool r = false;
		ASSERT_EQ(reader._scratch, 0x0);
		ASSERT_EQ(reader._scratch_bits, 0);
		ASSERT_EQ(reader._word_index, 0);

		uint32_t num1 {0};
		r = reader.serializeBits(num1, num1_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num1, num1_orig);
		ASSERT_EQ(reader._scratch, 0b00000000'00000000'00000000'00000000'000'11111111111111111111111111111);
		ASSERT_EQ(reader._scratch_bits, 29);
		ASSERT_EQ(reader._word_index, 1); // index refers to next dword

		uint32_t num2 {0};
		r = reader.serializeBits(num2, num2_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num2, num2_orig);
		ASSERT_EQ(reader._scratch, 0b00000000'00000000'00000000'00000000'000'00000'00000'1111111111111111111);
		ASSERT_EQ(reader._scratch_bits, 19);
		ASSERT_EQ(reader._word_index, 1); // <=32, so should not yet have read next dword

		uint32_t num3 {0};
		r = reader.serializeBits(num3, num3_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num3, num3_orig);
		ASSERT_EQ(reader._scratch, 0x0); // no data left
		ASSERT_EQ(reader._scratch_bits, 27);
		ASSERT_EQ(reader._word_index, 2);

		// error case
		uint32_t num4 {0};
		r = reader.serializeBits(num4, 32);
		ASSERT_FALSE(r);
	}
}

TEST(s6zer, stream_normalcase1_1) {
	const uint32_t num1_orig {0b101};
	const uint32_t num1_orig_bits {3};
	const uint32_t num2_orig {0b1010101010};
	const uint32_t num2_orig_bits {10};
	const uint32_t num3_orig {0b101010101010101010101010};
	const uint32_t num3_orig_bits {24};

	std::array<uint32_t, 2> buffer;
	size_t buffer_size = buffer.size()*sizeof(uint32_t);
	{
		MM::s6zer::StreamWriter writer{buffer.data(), buffer_size};

		bool r = false;
		ASSERT_EQ(writer._scratch, 0x0);
		ASSERT_EQ(writer._scratch_bits, 0);
		ASSERT_EQ(writer._word_index, 0);
		ASSERT_EQ(writer._bits_written, 0);
		ASSERT_EQ(writer.bytesWritten(), 0);

		r = writer.serializeBits(num1_orig, num1_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0b0000000000000000000000000000000000000000000000000000000000000'101);
		ASSERT_EQ(writer._scratch_bits, 3);
		ASSERT_EQ(writer._word_index, 0);
		ASSERT_EQ(writer._bits_written, num1_orig_bits);

		r = writer.serializeBits(num2_orig, num2_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0b000000000000000000000000000000000000000000000000000'1010101010'101);
		ASSERT_EQ(writer._scratch_bits, 3+10);
		ASSERT_EQ(writer._word_index, 0);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits);

		r = writer.serializeBits(num3_orig, num3_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0b00000000000000000000000000000000000000000000000000000000000'10101); // the high bits of the 24
		ASSERT_EQ(writer._scratch_bits, (3+10+24)-32);
		ASSERT_EQ(writer._word_index, 1);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits+num3_orig_bits);

		r = writer.flush();
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0);
		ASSERT_EQ(writer._scratch_bits, 0);
		ASSERT_EQ(writer._word_index, 2);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits+num3_orig_bits); // flush does not change bits written
		ASSERT_EQ(writer.bytesWritten(), 5); // 4.625 , so ceil

		buffer_size = writer.bytesWritten();
	}

	std::cout << "buffer_size: " << buffer_size << "\n";

	ASSERT_EQ(buffer[0], 0b0101010101010101010'1010101010'101);
	ASSERT_EQ(buffer[1], 0b000000000000000000000000000'10101);

	{
		MM::s6zer::StreamReader reader{buffer.data(), buffer_size};

		bool r = false;
		ASSERT_EQ(reader._scratch, 0x0);
		ASSERT_EQ(reader._scratch_bits, 0);
		ASSERT_EQ(reader._word_index, 0);

		uint32_t num1 {0};
		r = reader.serializeBits(num1, num1_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num1, num1_orig);
		ASSERT_EQ(reader._scratch, 0b00000000'00000000'00000000'00000000'000'0101010101010101010'1010101010);
		ASSERT_EQ(reader._scratch_bits, 29);
		ASSERT_EQ(reader._word_index, 1); // index refers to next dword

		uint32_t num2 {0};
		r = reader.serializeBits(num2, num2_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num2, num2_orig);
		ASSERT_EQ(reader._scratch, 0b00000000'00000000'00000000'00000000'000'00000'00000'0101010101010101010);
		ASSERT_EQ(reader._scratch_bits, 19);
		ASSERT_EQ(reader._word_index, 1); // <=32, so should not yet have read next dword

		uint32_t num3 {0};
		r = reader.serializeBits(num3, num3_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num3, num3_orig);
		ASSERT_EQ(reader._scratch, 0x0); // no data left
		ASSERT_EQ(reader._scratch_bits, 27);
		ASSERT_EQ(reader._word_index, 2);

		// error case
		uint32_t num4 {0};
		r = reader.serializeBits(num4, 32);
		ASSERT_FALSE(r);
	}
}

TEST(s6zer, stream_normalcase2) {
	// we now take each number as its maximum, synthetic for testing
	const uint32_t num1_orig {17};
	const uint32_t num1_orig_bits {MM::s6zer::bits_required(num1_orig)};
	const uint32_t num2_orig {1};
	const uint32_t num2_orig_bits {MM::s6zer::bits_required(num2_orig)};
	const uint32_t num3_orig {1298989};
	const uint32_t num3_orig_bits {MM::s6zer::bits_required(num3_orig)};

	std::array<uint32_t, 2> buffer;
	size_t buffer_size = buffer.size()*sizeof(uint32_t);
	{
		MM::s6zer::StreamWriter writer{buffer.data(), buffer_size};

		// fewer asserts
		bool r = false;
		ASSERT_EQ(writer._scratch, 0x0);
		ASSERT_EQ(writer._scratch_bits, 0);
		ASSERT_EQ(writer._word_index, 0);
		ASSERT_EQ(writer._bits_written, 0);
		ASSERT_EQ(writer.bytesWritten(), 0);

		r = writer.serializeBits(num1_orig, num1_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._bits_written, num1_orig_bits);

		r = writer.serializeBits(num2_orig, num2_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits);

		r = writer.serializeBits(num3_orig, num3_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits+num3_orig_bits);

		r = writer.flush();
		ASSERT_TRUE(r);
		ASSERT_EQ(writer._scratch, 0);
		ASSERT_EQ(writer._scratch_bits, 0);
		ASSERT_EQ(writer._bits_written, num1_orig_bits+num2_orig_bits+num3_orig_bits); // flush does not change bits written

		buffer_size = writer.bytesWritten();
	}

	std::cout << "buffer_size: " << buffer_size << "\n";

	{
		MM::s6zer::StreamReader reader{buffer.data(), buffer_size};

		bool r = false;
		ASSERT_EQ(reader._scratch, 0x0);
		ASSERT_EQ(reader._scratch_bits, 0);
		ASSERT_EQ(reader._word_index, 0);
		ASSERT_EQ(reader._bits_read, 0);

		uint32_t num1 {0};
		r = reader.serializeBits(num1, num1_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num1, num1_orig);

		uint32_t num2 {0};
		r = reader.serializeBits(num2, num2_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num2, num2_orig);

		uint32_t num3 {0};
		r = reader.serializeBits(num3, num3_orig_bits);
		ASSERT_TRUE(r);
		ASSERT_EQ(num3, num3_orig);

		// error case
		uint32_t num4 {0};
		r = reader.serializeBits(num4, 32);
		ASSERT_FALSE(r);
	}
}

// emscripten cant do this
#ifndef __EMSCRIPTEN__
bool serialize_int_death_fn(void) {
		std::array<uint32_t, 32> dummy_buffer;
		MM::s6zer::StreamReader reader{dummy_buffer.data(), dummy_buffer.size()*sizeof(uint32_t)};
		int32_t value{0};

		MM_S6ZER_BAIL(reader.serializeInt(value, 20, -20)); // wrong order

	return true;
}

TEST(s6zer, serialize_int_death) {
	ASSERT_DEATH({
		[[maybe_unused]] bool ret = serialize_int_death_fn();
	}, "failed");
}
#endif

TEST(s6zer, reader_bits_bug1) {
	const std::array<uint32_t, 2> buffer {
		0x27'5c'19'a1,
		0x00'00'3a'c3
	};
	const size_t buffer_size {6};

	MM::s6zer::StreamReader reader{buffer.data(), buffer_size};

	uint8_t value_0 {0};
	ASSERT_TRUE(reader.serializeBits(value_0));
	ASSERT_EQ(value_0, 0xa1);

	ASSERT_EQ(reader._scratch_bits, 24);
	ASSERT_EQ(reader._scratch, 0x0000000000'27'5c'19);

	uint32_t value_1 {0};
	ASSERT_TRUE(reader.serializeBits(value_1));
	ASSERT_EQ(value_1, 0xc3'27'5c'19);

	ASSERT_EQ(reader._scratch_bits, 24);
	ASSERT_EQ(reader._scratch, 0x00000000000000'3a);

	uint8_t value_2 {0};
	ASSERT_TRUE(reader.serializeBits(value_2));
	ASSERT_EQ(value_2, 0x3a);

	ASSERT_EQ(reader._scratch, 0x0000000000000000);
}

struct TestStruct {
	// integers bits
	uint8_t		u8 {0};
	uint16_t	u16 {0};
	uint32_t	u32 {0};
	//uint64_t	u64 {0};

	bool		b1 {false};

	// integers ranges
	uint8_t		r_u8 {0};
	constexpr static MM::ScalarRange2<uint8_t> r_u8_r{10, 60};
	int8_t		r_i8 {0};
	constexpr static MM::ScalarRange2<int8_t> r_i8_r{-10, 5};
	uint16_t	r_u16 {0};
	constexpr static MM::ScalarRange2<uint16_t> r_u16_r{1, 1026};
	int16_t		r_i16 {0};
	constexpr static MM::ScalarRange2<int16_t> r_i16_r{-1, 1026};
	uint32_t	r_u32 {0};
	constexpr static MM::ScalarRange2<uint32_t> r_u32_r{0, 12341234};
	int32_t		r_i32 {0};
	constexpr static MM::ScalarRange2<int32_t> r_i32_r{-12341234, 10};

	// floats
	float		f32 {0.f};
	double		f64 {0.};

	// float compressed [0; 1] range
	constexpr static float c0_f32_resolution = 0.001;
	constexpr static MM::ScalarRange2<float> c0_f32_r{0.f, 1.f};
	float		c0_f32_0 {0.f};
	float		c0_f32_1 {0.f};
	float		c0_f32_2 {0.f};
	float		c0_f32_3 {0.f};

	// float compressed [-1; 1] range
	constexpr static float c1_f32_resolution = 0.05;
	constexpr static MM::ScalarRange2<float> c1_f32_r{-1.f, 1.f};
	float		c1_f32_0 {0.f};
	float		c1_f32_1 {0.f};
	float		c1_f32_2 {0.f};
	float		c1_f32_3 {0.f};

	// float compressed [-1000; 1000] range
	constexpr static float c2_f32_resolution = 0.01;
	constexpr static MM::ScalarRange2<float> c2_f32_r{-1000.f, 1000.f};
	float		c2_f32_0 {0.f};
	float		c2_f32_1 {0.f};
	float		c2_f32_2 {0.f};
	float		c2_f32_3 {0.f};
};

MM_DEFINE_SERIALIZE(TestStruct,
	MM_S6ZER_BAIL(stream.serializeBits(data.u8))
	MM_S6ZER_BAIL(stream.serializeBits(data.u16))
	MM_S6ZER_BAIL(stream.serializeBits(data.u32))

	MM_S6ZER_BAIL(stream.serializeBool(data.b1))

	MM_S6ZER_BAIL(stream.serializeInt(data.r_u8, data.r_u8_r.min(), data.r_u8_r.max()))
	MM_S6ZER_BAIL(stream.serializeInt(data.r_i8, data.r_i8_r.min(), data.r_i8_r.max()))
	MM_S6ZER_BAIL(stream.serializeInt(data.r_u16, data.r_u16_r.min(), data.r_u16_r.max()))
	MM_S6ZER_BAIL(stream.serializeInt(data.r_i16, data.r_i16_r.min(), data.r_i16_r.max()))
	MM_S6ZER_BAIL(stream.serializeInt(data.r_u32, data.r_u32_r.min(), data.r_u32_r.max()))
	MM_S6ZER_BAIL(stream.serializeInt(data.r_i32, data.r_i32_r.min(), data.r_i32_r.max()))

	MM_S6ZER_BAIL(stream.serializeFloat(data.f32))
	MM_S6ZER_BAIL(stream.serializeDouble(data.f64))

	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c0_f32_0, data.c0_f32_r.min(), data.c0_f32_r.max(), data.c0_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c0_f32_1, data.c0_f32_r.min(), data.c0_f32_r.max(), data.c0_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c0_f32_2, data.c0_f32_r.min(), data.c0_f32_r.max(), data.c0_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c0_f32_3, data.c0_f32_r.min(), data.c0_f32_r.max(), data.c0_f32_resolution))

	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c1_f32_0, data.c1_f32_r.min(), data.c1_f32_r.max(), data.c1_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c1_f32_1, data.c1_f32_r.min(), data.c1_f32_r.max(), data.c1_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c1_f32_2, data.c1_f32_r.min(), data.c1_f32_r.max(), data.c1_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c1_f32_3, data.c1_f32_r.min(), data.c1_f32_r.max(), data.c1_f32_resolution))

	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c2_f32_0, data.c2_f32_r.min(), data.c2_f32_r.max(), data.c2_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c2_f32_1, data.c2_f32_r.min(), data.c2_f32_r.max(), data.c2_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c2_f32_2, data.c2_f32_r.min(), data.c2_f32_r.max(), data.c2_f32_resolution))
	MM_S6ZER_BAIL(stream.serializeFloatCompressed(data.c2_f32_3, data.c2_f32_r.min(), data.c2_f32_r.max(), data.c2_f32_resolution))
)

TEST(s6zer, stream_normalfull) {
	std::array<uint32_t, 128> buffer;
	size_t buffer_size = buffer.size()*sizeof(uint32_t);

	MM::Random::SRNG rng{1337, 0};

	const TestStruct data_in{
		static_cast<uint8_t>(rng()),
		static_cast<uint16_t>(rng()),
		static_cast<uint32_t>(rng()),

		rng.roll(0.5f),

		rng.range(TestStruct::r_u8_r),
		rng.range(TestStruct::r_i8_r),
		rng.range(TestStruct::r_u16_r),
		rng.range(TestStruct::r_i16_r),
		rng.range(TestStruct::r_u32_r),
		rng.range(TestStruct::r_i32_r),

		rng.negOneToOne() * 10000000.f,
		rng.negOneToOne() * 10000000.,

		rng.range(TestStruct::c0_f32_r),
		rng.range(TestStruct::c0_f32_r),
		rng.range(TestStruct::c0_f32_r),
		rng.range(TestStruct::c0_f32_r),

		rng.range(TestStruct::c1_f32_r),
		rng.range(TestStruct::c1_f32_r),
		rng.range(TestStruct::c1_f32_r),
		rng.range(TestStruct::c1_f32_r),

		rng.range(TestStruct::c2_f32_r),
		rng.range(TestStruct::c2_f32_r),
		rng.range(TestStruct::c2_f32_r),
		rng.range(TestStruct::c2_f32_r),
	};

	std::cout << "struct size: " << sizeof(TestStruct) << "\n";

	{
		MM::s6zer::StreamWriter writer{buffer.data(), buffer_size};

		ASSERT_TRUE(mm_serialize(writer, data_in));

		ASSERT_TRUE(writer.flush());
		buffer_size = writer.bytesWritten();
	}

	std::cout << "buffer_size: " << buffer_size << "\n";

	TestStruct data_out{}; // all zero

	{
		MM::s6zer::StreamReader reader{buffer.data(), buffer_size};

		ASSERT_TRUE(mm_serialize(reader, data_out));

		ASSERT_EQ(reader._scratch, 0x0000000000000000);
	}

	std::cout << "buffer: ";
	for (size_t i = 0; i < buffer_size; i++) {
		std::cout << std::hex << static_cast<uint32_t>(reinterpret_cast<uint8_t*>(buffer.data())[i]) << "'";
	}
	std::cout << "\n";

	//std::cout << "data_out: \n" << data_out;

	ASSERT_EQ(data_in.u8, data_out.u8);
	ASSERT_EQ(data_in.u16, data_out.u16);
	ASSERT_EQ(data_in.u32, data_out.u32);

	ASSERT_EQ(data_in.b1, data_out.b1);

	ASSERT_EQ(data_in.r_u8, data_out.r_u8) << "value range: " << TestStruct::r_u8_r;
	ASSERT_EQ(data_in.r_i8, data_out.r_i8) << "value range: " << TestStruct::r_i8_r;
	ASSERT_EQ(data_in.r_u16, data_out.r_u16) << "value range: " << TestStruct::r_u16_r;
	ASSERT_EQ(data_in.r_i16, data_out.r_i16) << "value range: " << TestStruct::r_i16_r;
	ASSERT_EQ(data_in.r_u32, data_out.r_u32) << "value range: " << TestStruct::r_u32_r;
	ASSERT_EQ(data_in.r_i32, data_out.r_i32) << "value range: " << TestStruct::r_i32_r;

	// bit perfect copies, can have wrong results for special values <.<
	ASSERT_EQ(data_in.f32, data_out.f32);
	ASSERT_EQ(data_in.f64, data_out.f64);

	ASSERT_NEAR(data_in.c0_f32_0, data_out.c0_f32_0, TestStruct::c0_f32_resolution);
	ASSERT_NEAR(data_in.c0_f32_1, data_out.c0_f32_1, TestStruct::c0_f32_resolution);
	ASSERT_NEAR(data_in.c0_f32_2, data_out.c0_f32_2, TestStruct::c0_f32_resolution);
	ASSERT_NEAR(data_in.c0_f32_3, data_out.c0_f32_3, TestStruct::c0_f32_resolution);

	ASSERT_NEAR(data_in.c1_f32_0, data_out.c1_f32_0, TestStruct::c1_f32_resolution);
	ASSERT_NEAR(data_in.c1_f32_1, data_out.c1_f32_1, TestStruct::c1_f32_resolution);
	ASSERT_NEAR(data_in.c1_f32_2, data_out.c1_f32_2, TestStruct::c1_f32_resolution);
	ASSERT_NEAR(data_in.c1_f32_3, data_out.c1_f32_3, TestStruct::c1_f32_resolution);

	ASSERT_NEAR(data_in.c2_f32_0, data_out.c2_f32_0, TestStruct::c2_f32_resolution);
	ASSERT_NEAR(data_in.c2_f32_1, data_out.c2_f32_1, TestStruct::c2_f32_resolution);
	ASSERT_NEAR(data_in.c2_f32_2, data_out.c2_f32_2, TestStruct::c2_f32_resolution);
	ASSERT_NEAR(data_in.c2_f32_3, data_out.c2_f32_3, TestStruct::c2_f32_resolution);
}

