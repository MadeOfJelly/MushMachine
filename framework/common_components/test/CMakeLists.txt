
add_executable(common_component_json_serialization_test
	component_json_serialization_test.cpp
)

target_include_directories(common_component_json_serialization_test PRIVATE ".")

target_link_libraries(common_component_json_serialization_test
	common_components_serialize_json

	gtest_main
)

add_test(NAME common_component_json_serialization_test COMMAND common_component_json_serialization_test)

########################################

add_executable(common_component_s6zer_serialization_test
	component_s6zer_serialization_test.cpp
)

target_include_directories(common_component_s6zer_serialization_test PRIVATE ".")

target_link_libraries(common_component_s6zer_serialization_test
	common_components_serialize_s6zer

	gtest_main
)

add_test(NAME common_component_s6zer_serialization_test COMMAND common_component_s6zer_serialization_test)

