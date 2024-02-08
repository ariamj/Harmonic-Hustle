#define WITH_MINIAUDIO
#include <soloud.h>
#include <soloud_wav.h>
#include <common.hpp>
#include <audio_system.hpp>
#include <iostream>

SoLoud::Soloud gSoloud;

SoLoud::Wav BGM;

AudioSystem::AudioSystem() {
	return;
}


AudioSystem::~AudioSystem() {
	gSoloud.stopAll();
	gSoloud.deinit();
	return;
}

bool AudioSystem::init() {
	gSoloud.init();
	LoadFiles();
	return true;
}

bool AudioSystem::LoadFiles() {
	int res = BGM.load(audio_path("music.wav").c_str());
	if (res != 0) {
		return false;
	}
	std::cout << res << "\n";
	return true; // Successfully loaded all files
}

bool AudioSystem::playBGM() {
	gSoloud.play(BGM);
	return true;
}