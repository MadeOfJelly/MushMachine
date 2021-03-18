#pragma once

namespace MM {

template<typename T>
struct ScalarRange2 {
	T v_min {};
	T v_max {};

	ScalarRange2(void) = default;

	ScalarRange2(const T& min, const T& max) noexcept {
		if (min <= max) {
			v_min = min;
			v_max = max;
		} else {
			v_min = max;
			v_max = min;
		}
	}

	bool operator==(const ScalarRange2<T>& rhs) const {
		return min() == rhs.min() && max() == rhs.max();
	}

	bool operator!=(const ScalarRange2<T>& rhs) const {
		return !(*this == rhs);
	}

	[[nodiscard]] T& min(void) { return v_min; }
	[[nodiscard]] T& max(void) { return v_max; }

	[[nodiscard]] const T& min(void) const { return v_min; }
	[[nodiscard]] const T& max(void) const { return v_max; }

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

	[[nodiscard]] bool inRange(T&& value) const {
		return value >= min() && value <= max();
	}
};

} // MM

