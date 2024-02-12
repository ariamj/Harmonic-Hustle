#include <common.hpp>
#include <audio_system.hpp>
#include <iostream>

AudioSystem::AudioSystem() {
	return;
}


AudioSystem::~AudioSystem() {
		// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	Mix_CloseAudio();

	return;
}

bool AudioSystem::init() {
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	LoadFiles();
	playBGM();
	return true;
}

bool AudioSystem::LoadFiles() {
	// Load BGM from file path
	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());

	if (background_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str());
		return false;
	}
	return true; // Successfully loaded all files
}

bool AudioSystem::playBGM() {
	// Playing background music indefinitely using SDL
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Playing background music\n");
	return true;
}