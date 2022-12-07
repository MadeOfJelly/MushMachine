#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint8_t, etc

#include <type_traits>

#include <cassert>

// TODO: make asserts redefinable

namespace MM::s6zer {

// this is heavily inspired by Glenn Fiedler's (Gaffer On Games) serializers
// https://www.gafferongames.com/post/reading_and_writing_packets/
// https://www.gafferongames.com/post/serialization_strategies/

// internal helpers
namespace detail {
	// TODO: ugly, replace when c++20
	[[nodiscard]] constexpr size_t first_bit_set8(const uint8_t number) {
		return
			(number & 0b10000000) ? 8 :
			(number & 0b01000000) ? 7 :
			(number & 0b00100000) ? 6 :
			(number & 0b00010000) ? 5 :
			(number & 0b00001000) ? 4 :
			(number & 0b00000100) ? 3 :
			(number & 0b00000010) ? 2 :
			(number & 0b00000001) ? 1 :
			0
		;
	}

	[[nodiscard]] constexpr size_t first_bit_set32(const uint32_t number) {
		return
			(number & 0xff000000) ? first_bit_set8((number >> 24) & 0xff) + 24 :
			(number & 0x00ff0000) ? first_bit_set8((number >> 16) & 0xff) + 16 :
			(number & 0x0000ff00) ? first_bit_set8((number >> 8) & 0xff) + 8 :
			(number & 0x000000ff) ? first_bit_set8(number & 0xff) :
			0
		;
	}

	[[nodiscard]] constexpr uint32_t byte_swap(const uint32_t value) noexcept {
		return
			((value & 0xff000000) >> 24) |
			((value & 0x00ff0000) >> 8) |
			((value & 0x0000ff00) << 8) |
			((value & 0x000000ff) << 24)
		;
	}

	[[nodiscard]] constexpr uint16_t byte_swap(const uint16_t value) noexcept {
		return
			((value & 0xff00) >> 8) |
			((value & 0x00ff) << 8)
		;
	}

	[[nodiscard]] constexpr uint8_t byte_swap(const uint8_t value) noexcept {
		// noop
		return value;
	}

	template<typename T>
	[[nodiscard]] constexpr const T& max(const T& a, const T& b) noexcept {
		return (a < b) ? b : a;
	}

} // detail

// TODO: maybe 64bit?
// TODO: is this detail?
[[nodiscard]] constexpr size_t bits_required(const uint32_t numbers) {
	return detail::first_bit_set32(numbers);
}

[[nodiscard]] constexpr uint32_t serialize_byte_order(const uint32_t value) {
	// TODO: only works on little endian for now
	if constexpr (true) { // native is little endian
		return value;
	} else { // native is big endian
		return detail::byte_swap(value);
	}
}

// helper for fake exceptions
#ifndef MM_S6ZER_BAIL
#define MM_S6ZER_BAIL(...) { \
	if (! __VA_ARGS__) { \
		return false; \
	} \
}
#endif

struct StreamWriter {
	StreamWriter(void) = delete;
	StreamWriter(uint32_t* data, size_t size) : _data(data), _data_size(size) {
		assert(size != 0);
		assert(size % sizeof(uint32_t) == 0);
		assert(data != nullptr);
	}

	// do i still need them?
	[[nodiscard]] static constexpr bool isWriting(void) noexcept { return true; }
	[[nodiscard]] static constexpr bool isReading(void) noexcept { return false; }

	[[nodiscard]] bool flush(void) noexcept {
		if (_scratch_bits != 0) {
			// check if space in buffer
			if (_data_size < (_word_index + 1) * sizeof(uint32_t)) {
				return false;
			}

			_data[_word_index] = serialize_byte_order(static_cast<uint32_t>(_scratch & 0xffffffff));
			_scratch >>= 32; // new bits are allways unset, so we can just allways 32
			// we dont like negative
			_scratch_bits = detail::max(static_cast<int32_t>(_scratch_bits) - 32, 0);
			_word_index++;
		}

		return true;
	}

	template<typename T>
	[[nodiscard]] bool serializeBits(const T value, const size_t number_of_bits = sizeof(T)*8) noexcept {
		static_assert(std::is_integral_v<T>, "type needs to be an integer");
		static_assert(std::is_unsigned_v<T>, "type needs to be unsigned");
		static_assert(sizeof(T) <= 4, "not yet defined for > 32bit");
		assert(number_of_bits <= sizeof(T)*8);
		assert(number_of_bits > 0);

		// do scratching
		_scratch |= static_cast<uint64_t>(value) << _scratch_bits;
		_scratch_bits += number_of_bits;
		_bits_written += number_of_bits;

		if (_scratch_bits >= 32) {
			return flush();
		}

		return true;
	}

	[[nodiscard]] bool serializeBool(const bool value) noexcept {
		return serializeBits(static_cast<uint32_t>(value), 1);
	}

	template<typename T>
	[[nodiscard]] bool serializeInt(const T value, const T min, const T max) noexcept {
		static_assert(std::is_integral_v<T>, "type needs to be an integer");
		static_assert(sizeof(T) <= 4, "not yet defined for > 32bit");
		assert(max >= min);
		assert(value >= min);
		assert(value <= max);

		const size_t bits = bits_required(max - min);

		return serializeBits(static_cast<uint32_t>(value - min), bits);
	}

	[[nodiscard]] bool serializeFloat(const float value) noexcept {
		// TODO: dont use loop
		for (size_t i = 0; i < sizeof(float); i++) {
			MM_S6ZER_BAIL(serializeBits(reinterpret_cast<const uint8_t*>(&value)[i], 8));
		}

		return true;
	}

	[[nodiscard]] bool serializeDouble(const double value) noexcept {
		// TODO: dont use loop
		for (size_t i = 0; i < sizeof(double); i++) {
			MM_S6ZER_BAIL(serializeBits(reinterpret_cast<const uint8_t*>(&value)[i], 8));
		}

		return true;
	}

	[[nodiscard]] bool serializeFloatCompressed(const float value, const float min, const float max, const float resolution) noexcept {
		assert(max >= min);
		assert(value >= min);
		assert(value <= max);

		// TODO: handle those rounding errors

		const float numbers = (max - min) / resolution;
		const size_t bits = bits_required(static_cast<uint32_t>(numbers));

		const uint32_t tmp_value = static_cast<uint32_t>((value - min) / resolution);

		return serializeBits(tmp_value, bits);
	}

	[[nodiscard]] size_t bytesWritten(void) noexcept {
		return (_bits_written+7) / 8;
	}

	uint32_t* _data {nullptr};
	size_t _data_size {0};

	uint64_t _scratch {0};
	size_t _scratch_bits {0};
	size_t _word_index {0};
	size_t _bits_written {0}; // includes bits still in scratch
};

struct StreamReader {
	StreamReader(void) = delete;
	// !! StreamReader assumes the data buffer has whole uint32_t,
	// so at the end, even though data_size might be less then 4 bytes,
	// here is actually a full, empty uint32
	// !! enable AddressSanitzier during development and testing
	StreamReader(const uint32_t* data, size_t size) : _data(data), _data_size(size) {
		assert(size != 0);
		//assert(size % sizeof(uint32_t) == 0);
		assert(data != nullptr);
	}

	// do i still need them?
	[[nodiscard]] static constexpr bool isWriting(void) noexcept { return false; }
	[[nodiscard]] static constexpr bool isReading(void) noexcept { return true; }

	template<typename T>
	[[nodiscard]] bool serializeBits(T& value, const size_t number_of_bits = sizeof(T)*8) noexcept {
		static_assert(std::is_integral_v<T>, "type needs to be an integer");
		static_assert(std::is_unsigned_v<T>, "type needs to be unsigned");
		static_assert(sizeof(T) <= 4, "not yet defined for > 32bit");
		assert(number_of_bits <= sizeof(T)*8);
		assert(number_of_bits > 0);

		if (_scratch_bits < number_of_bits) {
			if (_bits_read + number_of_bits > _data_size*8) {
				// would read past end
				return false;
			}

			_scratch |= static_cast<uint64_t>(serialize_byte_order(_data[_word_index])) << _scratch_bits;
			_word_index++;
			_scratch_bits += 32;
		}

		value = _scratch & ((uint64_t(1) << number_of_bits) - 1);

		_scratch >>= number_of_bits;
		_scratch_bits -= number_of_bits;
		_bits_read += number_of_bits;

		return true;
	}

	[[nodiscard]] bool serializeBool(bool& value) noexcept {
		uint32_t tmp_value {0};
		MM_S6ZER_BAIL(serializeBits(tmp_value, 1));

		// :)
		value = tmp_value != 0;

		return true;
	}

	template<typename T>
	[[nodiscard]] bool serializeInt(T& value, const T min, const T max) noexcept {
		static_assert(std::is_integral_v<T>, "type needs to be an integer");
		static_assert(sizeof(T) <= 4, "not yet defined for > 32bit");
		assert(max >= min);

		const size_t bits = bits_required(max - min);

		uint32_t tmp_val {0};
		MM_S6ZER_BAIL(serializeBits(tmp_val, bits));

		value = static_cast<T>(tmp_val) + min;

		return true;
	}

	[[nodiscard]] bool serializeFloat(float& value) noexcept {
		// TODO: dont use loop
		for (size_t i = 0; i < sizeof(float); i++) {
			MM_S6ZER_BAIL(serializeBits(reinterpret_cast<uint8_t*>(&value)[i], 8));
		}

		return true;
	}

	[[nodiscard]] bool serializeDouble(double& value) noexcept {
		// TODO: dont use loop
		for (size_t i = 0; i < sizeof(double); i++) {
			MM_S6ZER_BAIL(serializeBits(reinterpret_cast<uint8_t*>(&value)[i], 8));
		}

		return true;
	}

	[[nodiscard]] bool serializeFloatCompressed(float& value, const float min, const float max, const float resolution) noexcept {
		assert(max >= min);

		// TODO: use rounding, rn it snaps (floor)

		const float numbers = (max - min) / resolution;
		const size_t bits = bits_required(static_cast<uint32_t>(numbers));

		uint32_t tmp_value {0};
		MM_S6ZER_BAIL(serializeBits(tmp_value, bits));

		value = static_cast<float>(tmp_value) * resolution + min;

		return true;
	}

	[[nodiscard]] size_t bytesRead(void) noexcept {
		return (_bits_read+7) / 8;
	}

	const uint32_t* _data {nullptr};
	size_t _data_size {0};

	uint64_t _scratch {0};
	size_t _scratch_bits {0};
	size_t _word_index {0};
	size_t _bits_read {0};
};

} // MM::s6zer

