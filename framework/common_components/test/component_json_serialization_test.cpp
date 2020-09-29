#include <gtest/gtest.h>

#include <mm/components/serialize/json_name.hpp>
#include <mm/components/serialize/json_color.hpp>
#include <mm/components/serialize/json_transform2d.hpp>
#include <mm/components/serialize/json_transform3d.hpp>
#include <mm/components/serialize/json_velocity2d.hpp>
#include <mm/components/serialize/json_view_dir2d.hpp>
#include <mm/components/serialize/json_view_dir3d.hpp>

/*#define PARSE_TEST_MACRO(type, json_string, comp_val) \
TEST(common_components_json_serialization, type) { \
	MM::Components::type comp; \
	{ \
		auto j = nlohmann::json::parse(json_string); \
		EXPECT_NO_THROW({ comp = j; }); \
		ASSERT_EQ(comp, (comp_val)); \
	} \
	{ \
		nlohmann::json j; \
		[>EXPECT_NO_THROW({ j = comp; });<] \
		ASSERT_EQ(json_string, j.dump()); \
	} \
}

PARSE_TEST_MACRO(
	Name,
	"{\"str\":\"test_name\"}",
	MM::Components::Name{"test_name"}
); */

TEST(common_components_json_serialization, in_out_name) {
	MM::Components::Name comp;

	const char* json_test_file = "{\"str\":\"test_name\"}";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		EXPECT_NO_THROW({
			comp = j;
		});

		ASSERT_EQ(comp.str, "test_name");
	}

	{ // out
		nlohmann::json j;

		EXPECT_NO_THROW({
			j = comp;
		});

		ASSERT_EQ(json_test_file, j.dump());
	}
}

TEST(common_components_json_serialization, in_out_name_fail) {
	MM::Components::Name name_comp;

	// intentional malformed json string
	const char* json_test_file = "{\"strasdf\":\"test_name\"}";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		ASSERT_ANY_THROW({
			name_comp = j;
		});
	}
}

// ##############################################################

TEST(common_components_json_serialization, in_out_color) {
	MM::Components::Color comp;

	const char* json_test_file = R"({"color":{"w":1337.0,"x":0.0,"y":1.0,"z":3.0}})";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		EXPECT_NO_THROW({
			comp = j;
		});

		glm::vec4 comp_val{0.f, 1.f, 3.f, 1337.f};
		ASSERT_EQ(comp.color.x, comp_val.x);
		ASSERT_EQ(comp.color.y, comp_val.y);
		ASSERT_EQ(comp.color.z, comp_val.z);
		ASSERT_EQ(comp.color.w, comp_val.w);
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

TEST(common_components_json_serialization, in_out_transform2d) {
	MM::Components::Transform2D comp;

	const char* json_test_file = R"({"position":{"x":42.0,"y":6.0},"rotation":99.0,"scale":{"x":1337.0,"y":68.0}})";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		EXPECT_NO_THROW({
			comp = j;
		});

		ASSERT_EQ(comp.position.x, 42.f);
		ASSERT_EQ(comp.position.y, 6.f);

		ASSERT_EQ(comp.scale.x, 1337.f);
		ASSERT_EQ(comp.scale.y, 68.f);

		ASSERT_EQ(comp.rotation, 99.f);
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

TEST(common_components_json_serialization, in_out_transform3d) {
	MM::Components::Transform3D comp;

	const char* json_test_file = R"({"position":{"x":42.0,"y":6.0,"z":66.0},"rotation":99.0,"scale":{"x":1337.0,"y":68.0,"z":60.0}})";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		EXPECT_NO_THROW({
			comp = j;
		});

		ASSERT_EQ(comp.position.x, 42.f);
		ASSERT_EQ(comp.position.y, 6.f);
		ASSERT_EQ(comp.position.z, 66.f);

		ASSERT_EQ(comp.scale.x, 1337.f);
		ASSERT_EQ(comp.scale.y, 68.f);
		ASSERT_EQ(comp.scale.z, 60.f);

		// TODO: prob needs 3 rotations...
		ASSERT_EQ(comp.rotation, 99.f);
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

TEST(common_components_json_serialization, in_out_velocity2d) {
	MM::Components::Velocity2D comp;

	const char* json_test_file = R"({"rotation":99.0,"velocity":{"x":42.0,"y":6.0}})";

	{ // in
		auto j = nlohmann::json::parse(json_test_file);

		EXPECT_NO_THROW({
			comp = j;
		});

		ASSERT_EQ(comp.velocity.x, 42.f);
		ASSERT_EQ(comp.velocity.y, 6.f);

		ASSERT_EQ(comp.rotation, 99.f);
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

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

