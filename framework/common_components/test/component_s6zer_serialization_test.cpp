#include <gtest/gtest.h>

#include <array>

#include <mm/s6zer/serialize.hpp>

//#include <mm/components/serialize/json_name.hpp>
#include <mm/components/serialize/s6zer_color.hpp>

//#include <mm/components/serialize/json_position2d.hpp>
//#include <mm/components/serialize/json_position2d_zoffset.hpp>
//#include <mm/components/serialize/json_position3d.hpp>
//#include <mm/components/serialize/json_rotation2d.hpp>
//#include <mm/components/serialize/json_scale2d.hpp>
//#include <mm/components/serialize/json_transform4x4.hpp>
//#include <mm/components/serialize/json_velocity2d_position.hpp>
//#include <mm/components/serialize/json_velocity2d_position_intent.hpp>
//#include <mm/components/serialize/json_velocity2d_rotation.hpp>
//#include <mm/components/serialize/json_view_dir2d.hpp>
//#include <mm/components/serialize/json_view_dir3d.hpp>

#define TEST_JSON_SERL_EXPAND(x) x
#define TEST_JSON_SERL_IN_OUT(TYPE, JSON_STR, TEST_CORPUS) \
TEST(common_components_json_serialization, in_out_##TYPE) { \
	MM::Components::TYPE comp; \
	{ /* in */ \
		auto j = nlohmann::json::parse(JSON_STR); \
		comp = j; \
		TEST_CORPUS \
	} \
	{ /* out */ \
		nlohmann::json j; \
		j = comp; \
		ASSERT_EQ(JSON_STR, j.dump()); \
	} \
}


//TEST(common_components_json_serialization, in_out_name) {
	//MM::Components::Name comp;

	//const char* json_test_file = "{\"str\":\"test_name\"}";

	//{ // in
		//auto j = nlohmann::json::parse(json_test_file);

		//EXPECT_NO_THROW({
			//comp = j;
		//});

		//ASSERT_EQ(comp.str, "test_name");
	//}

	//{ // out
		//nlohmann::json j;

		//EXPECT_NO_THROW({
			//j = comp;
		//});

		//ASSERT_EQ(json_test_file, j.dump());
	//}
//}

//TEST(common_components_json_serialization, in_out_name_fail) {
	//MM::Components::Name name_comp;

	//// intentional malformed json string
	//const char* json_test_file = "{\"strasdf\":\"test_name\"}";

	//{ // in
		//auto j = nlohmann::json::parse(json_test_file);

		//ASSERT_ANY_THROW({
			//name_comp = j;
		//});
	//}
//}

// ##############################################################

//TEST_S6ZER_SERL_IN_OUT(
	//Color,
	//R"({"color":{"w":1337.0,"x":0.0,"y":1.0,"z":3.0}})",
	//TEST_JSON_SERL_EXPAND({
		//glm::vec4 comp_val(0.f, 1.f, 3.f, 1337.f);
		//ASSERT_EQ(comp.color.x, comp_val.x);
		//ASSERT_EQ(comp.color.y, comp_val.y);
		//ASSERT_EQ(comp.color.z, comp_val.z);
		//ASSERT_EQ(comp.color.w, comp_val.w);
	//})
//)

TEST(common_components_s6zer_serialization, out_in_color) {
	const MM::Components::Color comp_out{
		{1337.f, 0.f, 1.f, 3.f}
	};

	std::array<uint32_t, 128> buffer;
	size_t buffer_size = buffer.size()*sizeof(uint32_t);

	{ // to bits
		MM::s6zer::StreamWriter writer{buffer.data(), buffer_size};
		ASSERT_TRUE(MM::Components::mm_serialize(writer, comp_out));

		ASSERT_TRUE(writer.flush());
		buffer_size = writer.bytesWritten();
	}

	MM::Components::Color comp_in;

	{ // from bits
		MM::s6zer::StreamReader reader{buffer.data(), buffer_size};

		ASSERT_TRUE(MM::Components::mm_serialize(reader, comp_in));

		ASSERT_EQ(reader.bytesRead(), buffer_size);
	}

	ASSERT_EQ(comp_out.color.x, comp_in.color.x);
	ASSERT_EQ(comp_out.color.y, comp_in.color.y);
	ASSERT_EQ(comp_out.color.z, comp_in.color.z);
	ASSERT_EQ(comp_out.color.w, comp_in.color.w);
}

#if 0

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Position2D,
	R"({"pos":{"x":42.0,"y":6.0}})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.pos.x, 42.f);
		ASSERT_EQ(comp.pos.y, 6.f);
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Position2D_ZOffset,
	R"({"z_offset":3.0})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.z_offset, 3.f);
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Position3D,
	R"({"pos":{"x":42.0,"y":6.0,"z":44.0}})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.pos.x, 42.f);
		ASSERT_EQ(comp.pos.y, 6.f);
		ASSERT_EQ(comp.pos.z, 44.f);
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Rotation2D,
	R"({"rot":42.0})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.rot, 42.f);
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Scale2D,
	R"({"scale":{"x":42.0,"y":6.0}})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.scale.x, 42.f);
		ASSERT_EQ(comp.scale.y, 6.f);
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Transform4x4,
	R"({"trans":[{"w":0.0,"x":1.0,"y":0.0,"z":0.0},{"w":0.0,"x":0.0,"y":1.0,"z":0.0},{"w":0.0,"x":0.0,"y":0.0,"z":1.0},{"w":1.0,"x":0.0,"y":0.0,"z":0.0}]})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.trans, glm::mat4x4{1.f});
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Velocity2DPosition,
	R"({"pos_vel":{"x":42.0,"y":6.0}})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.pos_vel.x, 42.f);
		ASSERT_EQ(comp.pos_vel.y, 6.f);
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Velocity2DPositionIntent,
	R"({"intent":{"x":42.0,"y":6.0}})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.intent.x, 42.f);
		ASSERT_EQ(comp.intent.y, 6.f);
	)
)

// ##############################################################

TEST_JSON_SERL_IN_OUT(
	Velocity2DRotation,
	R"({"rot_vel":42.0})",
	TEST_JSON_SERL_EXPAND(
		ASSERT_EQ(comp.rot_vel, 42.f);
	)
)

// ##############################################################

TEST(common_components_json_serialization, in_out_view_dir2d) {
	MM::Components::ViewDir2D comp;

	const char* json_test_file = R"({"dir":6.0})";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		EXPECT_NO_THROW({
			comp = j;
		});

		ASSERT_EQ(comp.dir, 6.f);
	}

	{ // out
		nlohmann::json j;

		EXPECT_NO_THROW({
			j = comp;
		});

		ASSERT_EQ(json_test_file, j.dump());
	}
}

// ##############################################################

TEST(common_components_json_serialization, in_out_view_dir3d) {
	MM::Components::ViewDir3D comp;

	const char* json_test_file = R"({"pitch":6.0,"roll":99.0,"yaw":42.0})";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		EXPECT_NO_THROW({
			comp = j;
		});

		ASSERT_EQ(comp.yaw, 42.f);
		ASSERT_EQ(comp.pitch, 6.f);
		ASSERT_EQ(comp.roll, 99.f);
	}

	{ // out
		nlohmann::json j;

		EXPECT_NO_THROW({
			j = comp;
		});

		ASSERT_EQ(json_test_file, j.dump());
	}
}

#endif

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

