#pragma once

namespace MM {

template<typename T>
struct ScalarRange2 {
	T v_min {};
	T v_max {};

	constexpr ScalarRange2(void) = default;

	constexpr ScalarRange2(const T& both) noexcept {
		v_min = both;
		v_max = both;
	}

	constexpr ScalarRange2(const T& min, const T& max) noexcept {
		if (min <= max) {
			v_min = min;
			v_max = max;
		} else {
			v_min = max;
			v_max = min;
		}
	}

	constexpr bool operator==(const ScalarRange2<T>& rhs) const {
		return min() == rhs.min() && max() == rhs.max();
	}

	constexpr bool operator!=(const ScalarRange2<T>& rhs) const {
		return !(*this == rhs);
	}

	[[nodiscard]] constexpr T& min(void) { return v_min; }
	[[nodiscard]] constexpr T& max(void) { return v_max; }

	[[nodiscard]] constexpr const T& min(void) const { return v_min; }
	[[nodiscard]] constexpr const T& max(void) const { return v_max; }

	void setMin(const T& new_min) {
		min() = new_min;
		max() = max() < new_min ? new_min : max();
	}

	void setMax(const T& new_max) {
		max() = new_max;
		min() = min() > new_max ? new_max : min();
	}

	void sanitize(void) {
		if (min() > max()) {
			// TODO: is copy ok?
			T tmp = min();
			min() = max();
			max() = tmp;
		}
	}

	[[nodiscard]] constexpr bool inRange(T&& value) const {
		return value >= min() && value <= max();
	}

	// lerp between min and max
	[[nodiscard]] constexpr T map(const float a) const {
		return min() * (1.f-a) + max() * a;
	}

	// reverse map
	[[nodiscard]] constexpr float unmap(const T& v) const {
		return (v - min()) / static_cast<float>(max() - min());
	}
};

} // MM

