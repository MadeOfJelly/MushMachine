#include <gtest/gtest.h>

#include <mm/services/sound_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/soloud_filesystem_file_impl.hpp>

#include <SDL.h> // SDL_main

#include <soloud_wav.h>
#include <soloud_wavstream.h>
#include <soloud_monotone.h>

#include <thread>

char* argv0;

TEST(soloud_fs_file, basic) {
	MM::Engine engine;

	// setup
	auto& sound = engine.addService<MM::Services::SoundService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SoundService>());

	auto& fs = engine.addService<MM::Services::FilesystemService>(argv0, "soloud_filesystem_file_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	sound.engine.setGlobalVolume(0.4f);

	{
		const char* file_path = "does_not_exist.wav";
		ASSERT_FALSE(fs.exists(file_path));
		MM::SoLoud::FilesystemFile sl_f(fs.open(file_path), engine);

		SoLoud::Wav w;
		ASSERT_EQ(w.loadFile(&sl_f), SoLoud::SOLOUD_ERRORS::FILE_LOAD_FAILED);
	}

	while (sound.engine.getActiveVoiceCount()) {
	}
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

