#include <common.hpp>
#include <audio_system.hpp>
#include <iostream>

AudioSystem::AudioSystem() {
	return;
}


AudioSystem::~AudioSystem() {
		// Destroy music components
	if (overworld_music != nullptr)
		Mix_FreeMusic(overworld_music);
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
	// Play overworld by default on game initialization.
	return true;
}

bool AudioSystem::LoadFiles() {
	// Load BGM from file path
	std::string overworld_file = "overworld.wav";
	overworld_music = Mix_LoadMUS(audio_path(overworld_file).c_str());
	if (overworld_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(overworld_file).c_str());
		return false;
	}

	std::string enemy0_file = "enemy0.wav";
	Mix_Music* enemy0_music = Mix_LoadMUS(audio_path(enemy0_file).c_str());
	if (enemy0_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(enemy0_file).c_str());
		return false;
	}
	enemy_music.push_back(enemy0_music);
	

	std::string enemy1_file = "enemy1.1.wav";
	Mix_Music* enemy1_music = Mix_LoadMUS(audio_path(enemy1_file).c_str());
	if (enemy1_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(enemy1_file).c_str());
		return false;
	}
	enemy_music.push_back(enemy1_music);



	std::string enemy2_file = "enemy2.wav";
	Mix_Music* enemy2_music = Mix_LoadMUS(audio_path(enemy2_file).c_str());
	if (enemy2_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(enemy2_file).c_str());
		return false;
	}
	enemy_music.push_back(enemy2_music);

	//TODO: ADD NEW SONG FOR THE BOSS
	enemy_music.push_back(enemy2_music);

	std::string drop = "drop.wav";
	drop_SFX = Mix_LoadWAV(audio_path(drop).c_str());
	if (drop_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(drop).c_str());
		return false;
	}

	std::string hit_perfect = "hit_perfect.wav";
	hit_perfect_SFX = Mix_LoadWAV(audio_path("hit_perfect.wav").c_str());
	if (hit_perfect_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(hit_perfect).c_str());
		return false;
	}

	std::string miss = "miss.wav";
	miss_SFX = Mix_LoadWAV(audio_path(miss).c_str());
	if (miss_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(miss).c_str());
		return false;
	}

	return true; // Successfully loaded all files
}

bool AudioSystem::playOverworld() {
	// Use "Music" type and associated methods for background music (allegedly higher quality)
		// Will Use "Chunk" type for short SFX snippets later on
	Mix_PlayMusic(overworld_music, -1);
	fprintf(stderr, "Playing background music\n");

	current_music = overworld_music;
	return true;
}

bool AudioSystem::playBattle(int enemy_id) {
	// enemy_music is a vector of audio file data
	// Mix_PlayMusic(enemy_music[enemy_id], -1);
	Mix_PlayMusic(enemy_music[enemy_id], 1); // TEMP !!! FOR TESTING END
	current_music = enemy_music[enemy_id];
	return true;
}

bool AudioSystem::playDroppedNote() {
	Mix_PlayChannel(-1, drop_SFX, 0);
	return true;
}

bool AudioSystem::playMissedNote() {
	Mix_PlayChannel(-1, miss_SFX, 0);
	return true;
}

bool AudioSystem::playHitPerfect() {
	Mix_PlayChannel(-1, hit_perfect_SFX, 0);
	return true;
}

bool AudioSystem::musicPlaying() {
	// return 1 if music still playing, 0 otherwise
	return Mix_PlayingMusic();
}

float AudioSystem::getSongPosition() {
	return Mix_GetMusicPosition(current_music);
}