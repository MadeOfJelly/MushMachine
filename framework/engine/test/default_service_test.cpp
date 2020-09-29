#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/default_service.hpp>

TEST(engine_default_service_system, add_en_dis) {

	MM::Engine e;

	e.addService<MM::Services::DefaultService>();
	ASSERT_TRUE(e.enableService<MM::Services::DefaultService>());

	{
		auto* dss = e.tryService<MM::Services::DefaultService>();
		ASSERT_NE(dss, nullptr);
	}

	e.disableService<MM::Services::DefaultService>();
}

TEST(engine_default_service_system, tick) {

	MM::Engine e;

	e.addService<MM::Services::DefaultService>();
	ASSERT_TRUE(e.enableService<MM::Services::DefaultService>());

	e.update();
	e.fixedUpdate();

	{
		auto* dss = e.tryService<MM::Services::DefaultService>();
		ASSERT_NE(dss, nullptr);
	}

	e.disableService<MM::Services::DefaultService>();
}

