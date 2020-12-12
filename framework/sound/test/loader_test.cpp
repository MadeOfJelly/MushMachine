#include <gtest/gtest.h>

#include <mm/services/sound_service.hpp>
#include <mm/services/filesystem.hpp>
#include <physfs.h>
#include <mm/soloud_filesystem_file_impl.hpp>
#include <mm/sound_loader_wav.hpp>

#include <mm/resource_manager.hpp>

#include <soloud_wav.h>
#include <soloud_wavstream.h>
#include <soloud_monotone.h>

#include <thread>

// inlining zip
#include "res/erik_gun_fx_1.zip.h"

using namespace entt::literals;

extern char* argv0;

TEST(soloud_fs_loader, basic) {
	MM::Engine engine;

	// setup
	auto& sound = engine.addService<MM::Services::SoundService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SoundService>());

	engine.addService<MM::Services::FilesystemService>(argv0, "soloud_filesystem_loader_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rm = MM::ResourceManager<SoLoud::Wav>::ref();

	ASSERT_TRUE(PHYSFS_mountMemory(erik_gun_fx_1_zip, erik_gun_fx_1_zip_len, NULL, "", NULL, 0));


	sound.engine.setGlobalVolume(0.4f);


	ASSERT_FALSE(rm.contains("test"_hs));
	rm.load<MM::SoundLoaderWavFile>("test", "does_not_exist.wav", engine);
	ASSERT_FALSE(rm.contains("test"_hs));

	rm.load<MM::SoundLoaderWavFile>("test", "erik_gun_fx_1.flac", engine);
	ASSERT_TRUE(rm.contains("test"_hs));

	auto wh = rm.get("test"_hs);

	std::cout << "wav length: " << wh->getLength() << "s" << std::endl;
	sound.engine.play(*wh);

	while (sound.engine.getActiveVoiceCount()) {
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5ms);
	}

	rm.clear();
}

