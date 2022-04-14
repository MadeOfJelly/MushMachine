#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/opengl_renderer.hpp>

//#include <mm/opengl/res/shaders_builtin.hpp>

#include <mm/opengl/shader.hpp>
#include <mm/opengl/shader_builder.hpp>

#include <mm/fs_const_archiver.hpp> // include only works on files rn

static const char* argv0 = "";

TEST(builtins, all) {
	MM::Engine engine;

	engine.addService<MM::Services::FilesystemService>(argv0);
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>()); // adds builtins

	engine.update();

	FS_CONST_MOUNT_FILE("/shaders/test_frag.glsl",
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

#include "/shaders/builtin/sampling.glsl"
#include "/shaders/builtin/tonemapping.glsl"
#include "/shaders/builtin/hashing.glsl"
#include "/shaders/builtin/noise.glsl"

void main() {
}
)");

	auto sb = MM::OpenGL::ShaderBuilder::start();
	sb.addStageVertex("void main() { gl_Position = vec4(0.0); }");
	sb.addStageFragmentF(engine, "/shaders/test_frag.glsl");
	auto shader = sb.finish();
	ASSERT_TRUE(shader);
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

