#include <gtest/gtest.h>

#include <mm/services/sound_service.hpp>

#include <soloud_sfxr.h>
#include <soloud_speech.h>

char* argv0; // HACK

TEST(sound_service, add_en_dis) {
	MM::Engine engine;

	engine.addService<MM::Services::SoundService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SoundService>());

	auto* sound_ss_ptr = engine.tryService<MM::Services::SoundService>();
	ASSERT_NE(sound_ss_ptr, nullptr);

	auto& sound = *sound_ss_ptr;

	sound.printErrorString(SoLoud::SOLOUD_ERRORS::DLL_NOT_FOUND);
	sound.printErrorString(SoLoud::SOLOUD_ERRORS::FILE_LOAD_FAILED);
	sound.printErrorString(SoLoud::SOLOUD_ERRORS::FILE_NOT_FOUND);
	sound.printErrorString(SoLoud::SOLOUD_ERRORS::INVALID_PARAMETER);
	sound.printErrorString(SoLoud::SOLOUD_ERRORS::NOT_IMPLEMENTED);
	sound.printErrorString(SoLoud::SOLOUD_ERRORS::OUT_OF_MEMORY);
	sound.printErrorString(SoLoud::SOLOUD_ERRORS::SO_NO_ERROR);
	sound.printErrorString(SoLoud::SOLOUD_ERRORS::UNKNOWN_ERROR);

	engine.disableService<MM::Services::SoundService>();
}

TEST(sound_service, sfxr_test) {
	MM::Engine engine;

	engine.addService<MM::Services::SoundService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SoundService>());

	auto* sound_ss_ptr = engine.tryService<MM::Services::SoundService>();
	ASSERT_NE(sound_ss_ptr, nullptr);

	auto& sound = *sound_ss_ptr;

	SoLoud::Sfxr fx1;

	for (int i = SoLoud::Sfxr::COIN; i < 10; i++) {
		fx1.loadPreset(i, 0);

		sound.engine.play(fx1);

		while (sound.engine.getActiveVoiceCount()) {
		}
	}

	engine.disableService<MM::Services::SoundService>();
}

TEST(sound_service, speech_test) {
	MM::Engine engine;

	engine.addService<MM::Services::SoundService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SoundService>());

	auto* sound_ss_ptr = engine.tryService<MM::Services::SoundService>();
	ASSERT_NE(sound_ss_ptr, nullptr);

	auto& sound = *sound_ss_ptr;

	SoLoud::Speech sp;

	sp.setText("I am the ship computer.");

	sound.engine.play(sp);

	while (sound.engine.getActiveVoiceCount()) {
	}

	engine.disableService<MM::Services::SoundService>();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
