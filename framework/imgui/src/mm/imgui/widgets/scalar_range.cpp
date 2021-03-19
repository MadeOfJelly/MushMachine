#include "./scalar_range.hpp"

namespace MM::ImGuiWidgets {

	// unsigned int
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint8_t>& range, uint8_t v_min , uint8_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_U8, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint16_t>& range, uint16_t v_min , uint16_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_U16, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint32_t>& range, uint32_t v_min , uint32_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_U32, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint64_t>& range, uint64_t v_min , uint64_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_U64, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

	// signed int
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int8_t>& range, int8_t v_min , int8_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_S8, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int16_t>& range, int16_t v_min , int16_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_S16, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int32_t>& range, int32_t v_min , int32_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_S32, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int64_t>& range, int64_t v_min , int64_t v_max) {
		return DragScalarRange2EXT(label, ImGuiDataType_S64, &range.min(), &range.max(), 0.1f, v_min, v_max);
	}

} // MM::ImGuiWidgets

