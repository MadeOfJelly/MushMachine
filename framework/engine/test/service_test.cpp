#include <gtest/gtest.h>

#include <mm/engine.hpp>

class TestService1 : public MM::Services::Service {
	public:
		const char* name(void) override { return "TestService1"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}
};

class TestService2 : public MM::Services::Service {
	public:
		const char* name(void) override { return "TestService2"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}
};

class TestService2Derived : public TestService2 {
	public:
		const char* name(void) override { return "TestService2Derived"; }

		bool enable(MM::Engine&) override { return true; }
		void disable(MM::Engine&) override {}
};

TEST(engine_service, add) {
	MM::Engine e;

	e.addService<TestService1>();

	auto* tss = e.tryService<TestService1>();
	ASSERT_NE(tss, nullptr);
}

TEST(engine_service, get_fail) {
	MM::Engine e;

	auto* tss = e.tryService<TestService1>();
	ASSERT_EQ(tss, nullptr);
}

#ifndef NDEBUG
TEST(engine_service, _fail) {
	MM::Engine e;

	e.addService<TestService1>();

	ASSERT_DEATH({
		e.addService<TestService1>(); // adding the same
	}, "failed");

	//auto* tss = e.tryService<TestService1>();
	//ASSERT_EQ(tss, nullptr);
}
#endif

TEST(engine_service, add_int_en_dis) {
	MM::Engine e;

	{
		auto& tss = e.addService<TestService1>();
		ASSERT_TRUE(tss.enable(e));
	}

	{
		auto* tss = e.tryService<TestService1>();
		ASSERT_NE(tss, nullptr);

		tss->disable(e);
	}
}

TEST(engine_service, add_en_dis) {
	MM::Engine e;

	e.addService<TestService1>();
	ASSERT_TRUE(e.enableService<TestService1>());

	{
		auto* tss = e.tryService<TestService1>();
		ASSERT_NE(tss, nullptr);
	}

	e.disableService<TestService1>();
}

TEST(engine_service, add_en_dis_mult) {
	MM::Engine e;

	e.addService<TestService1>();
	ASSERT_TRUE(e.enableService<TestService1>());

	e.addService<TestService2>();
	ASSERT_TRUE(e.enableService<TestService2>());

	{
		auto* tss = e.tryService<TestService1>();
		ASSERT_NE(tss, nullptr);
	}

	{
		auto* tss = e.tryService<TestService2>();
		ASSERT_NE(tss, nullptr);
	}

	e.disableService<TestService1>();
	e.disableService<TestService2>();
}

TEST(engine_service, provide) {
	MM::Engine e;

	e.addService<TestService2Derived>();
	ASSERT_TRUE(e.enableService<TestService2Derived>());

	bool r;

	// "mark" TestService2Derived as provider for TestService2
	r = e.provide<TestService2, TestService2Derived>(); ASSERT_TRUE(r);

	// should not compile
	//r = e.provide<TestService2Derived, TestService2>();
	//r = e.provide<TestService2, TestService2>();
	//r = e.provide<TestService2Derived, TestService2Derived>();

	{
		auto* tdss = e.tryService<TestService2Derived>();
		ASSERT_NE(tdss, nullptr);

		auto* iss = e.tryService<TestService2>();
		ASSERT_NE(iss, nullptr);

		// they are the same (ptr)
		ASSERT_EQ(tdss, iss);
	}

	e.disableService<TestService2Derived>();
}

TEST(engine_service, type_ids) {
	MM::Engine e;

	auto tss1_id = e.type<TestService1>();
	auto tss2_id = e.type<TestService2>();
	auto tss2d_id = e.type<TestService2Derived>();

	ASSERT_NE(tss1_id, tss2_id);
	ASSERT_NE(tss1_id, tss2d_id);
	ASSERT_NE(tss2_id, tss2d_id);
}

TEST(engine_service, add_en_dis_mult_type_ids) {
	MM::Engine e;

	auto tss1_id = e.type<TestService1>();
	auto tss2_id = e.type<TestService2>();
	ASSERT_NE(tss1_id, tss2_id);

	e.addService<TestService1>();
	ASSERT_TRUE(e.enableService(tss1_id));

	e.addService<TestService2>();
	ASSERT_TRUE(e.enableService(tss2_id));

	{
		auto* tss1 = e.tryService<TestService1>();
		ASSERT_NE(tss1, nullptr);

		auto* tss2 = e.tryService<TestService2>();
		ASSERT_NE(tss2, nullptr);

		ASSERT_NE((void*)tss1, (void*)tss2);
	}

	e.disableService(tss1_id);
	e.disableService<TestService2>(); // mixin'
}

TEST(engine_service, provide_type_ids) {
	MM::Engine e;

	auto tss2_id = e.type<TestService2>();
	auto tss2d_id = e.type<TestService2Derived>();

	e.addService<TestService2Derived>();
	ASSERT_TRUE(e.enableService(tss2d_id));

	bool r;

	// "mark" TestService2Derived as provider for TestService2
	r = e.provide(tss2_id, tss2d_id); ASSERT_TRUE(r); // typeid variant
	//r = e.provide<TestService2, TestService2Derived>(); ASSERT_TRUE(r);

	{
		auto* tdss = e.tryService<TestService2Derived>();
		ASSERT_NE(tdss, nullptr);

		auto* iss = e.tryService<TestService2>();
		ASSERT_NE(iss, nullptr);

		// they are the same (ptr)
		ASSERT_EQ((void*)tdss, (void*)iss);
	}

	e.disableService(tss2d_id);
}

TEST(engine_service, run) {
	MM::Engine e;

	e.addService<TestService1>();
	ASSERT_TRUE(e.enableService<TestService1>());

	{
		auto* tss = e.tryService<TestService1>();
		ASSERT_NE(tss, nullptr);
	}

	e.addFixedUpdate([](auto& e) { e.stop(); });

	e.run();

	e.disableService<TestService1>();
}

