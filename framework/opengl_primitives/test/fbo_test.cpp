#include <gtest/gtest.h>

#include <mm/resource_manager.hpp>

#include <mm/services/sdl_service.hpp>

#include <mm/opengl/frame_buffer_object.hpp>
#include <mm/opengl/fbo_builder.hpp>

using FBO = MM::OpenGL::FrameBufferObject;
using FBOBuilder = MM::OpenGL::FBOBuilder;
using Texture = MM::OpenGL::Texture;

class FBOTest : public ::testing::Test {
	protected:
		MM::Engine engine;

		void SetUp(void) override {
			// setup sdl
			auto& sdl_s = engine.addService<MM::Services::SDLService>();
			ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

			sdl_s.createGLWindow("rendering_fbo_test", 800, 600);
			ASSERT_NE(sdl_s.win, nullptr);
		}
};

TEST_F(FBOTest, builder_fail) {
	auto fbo = MM::OpenGL::FBOBuilder::start()
		.finish();

	ASSERT_FALSE(fbo);
}

TEST_F(FBOTest, builder_basic) {
	auto[w, h] = engine.getService<MM::Services::SDLService>().getWindowSize();

	{
		auto tex = Texture::createEmpty(GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE);
		ASSERT_TRUE(tex);

		auto fbo = FBOBuilder::start()
			.attachTexture(tex)
			.finish();

		ASSERT_TRUE(fbo);
	}
	{
		auto fbo = FBOBuilder::start()
			.attachTexture(Texture::createEmpty(GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE))
			.finish();

		ASSERT_TRUE(fbo);
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

