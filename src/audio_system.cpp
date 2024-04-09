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
	if(enemy_music[0] != nullptr) 
		Mix_FreeMusic(enemy_music[0]);
	if(enemy_music[1] != nullptr) 
		Mix_FreeMusic(enemy_music[1]);
	if(enemy_music[2] != nullptr) 
		Mix_FreeMusic(enemy_music[2]);
	if (drop_SFX != nullptr)
		Mix_FreeChunk(drop_SFX);
	if (overworld_music != nullptr)
		Mix_FreeChunk(hit_perfect_SFX);
	if (overworld_music != nullptr)
		Mix_FreeChunk(miss_SFX);
	Mix_CloseAudio();

	return;
}

bool AudioSystem::init() {
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return false;
	}
	// Increase number of channels (default is only 8)
	Mix_AllocateChannels(16);

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

	std::string lobby_file = "lobby.wav";
	lobby_music = Mix_LoadMUS(audio_path(lobby_file).c_str());
	if (lobby_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(lobby_file).c_str());
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
	

	std::string enemy1_file = "enemy1.wav";
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

	std::string boss0_file = "boss0.wav";
	Mix_Music* boss0_music = Mix_LoadMUS(audio_path(boss0_file).c_str());
	if (boss0_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(boss0_file).c_str());
		return false;
	}
	enemy_music.push_back(boss0_music);

	std::string drop = "drop.wav";
	drop_SFX = Mix_LoadWAV(audio_path(drop).c_str());
	if (drop_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(drop).c_str());
		return false;
	}

	std::string hit_good = "hit_good.wav";
	hit_good_SFX = Mix_LoadWAV(audio_path("hit_good.wav").c_str());
	if (hit_good_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(hit_good).c_str());
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
	
	std::string hold = "hold.wav";
	hold_SFX = Mix_LoadWAV(audio_path(hold).c_str());
	if (hold_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(hold).c_str());
		return false;
	}

	std::string countdown_high = "countdown_high.wav";
	countdown_high_SFX = Mix_LoadWAV(audio_path(countdown_high).c_str());
	if (countdown_high_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(countdown_high).c_str());
		return false;
	}

	std::string countdown_low = "countdown_low.wav";
	countdown_low_SFX = Mix_LoadWAV(audio_path(countdown_low).c_str());
	if (countdown_low_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(countdown_low).c_str());
		return false;
	}

	std::string applause = "applause.wav";
	applause_SFX = Mix_LoadWAV(audio_path(applause).c_str());
	if (applause_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(applause).c_str());
		return false;
	}

	std::string mode_change = "mode_change.wav";
	mode_change_SFX = Mix_LoadWAV(audio_path(mode_change).c_str());
	if (mode_change_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(mode_change).c_str());
		return false;
	}

	std::string mode_countdown = "mode_countdown.wav";
	mode_countdown_SFX = Mix_LoadWAV(audio_path(mode_countdown).c_str());
	if (mode_countdown_SFX == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path(mode_countdown).c_str());
		return false;
	}

	return true; // Successfully loaded all files
}

int AudioSystem::playOverworld() {
	// Don't reset music
	if (current_music == overworld_music) {
		return 0; 
	}
	current_music = overworld_music;
	return Mix_PlayMusic(overworld_music, -1);
}

int AudioSystem::playBattle(int enemy_id) {
	// enemy_music is a vector of audio file data
	// Mix_PlayMusic(enemy_music[enemy_id], -1);
	current_music = enemy_music[enemy_id];
	return Mix_PlayMusic(enemy_music[enemy_id], 1); // TEMP !!! FOR TESTING END
}

int AudioSystem::playLobby() {
	current_music = lobby_music;
	return Mix_PlayMusic(lobby_music, -1);
}

int AudioSystem::playHoldNote(int channel_offset) {
	// Volume was getting set to 0 between playthroughs somehow
	// This is most straightforward fix, but can be improved
	Mix_Volume(HOLD_NOTE_FIRST_CHANNEL + channel_offset, MIX_MAX_VOLUME);
	return Mix_PlayChannel(HOLD_NOTE_FIRST_CHANNEL + channel_offset, hold_SFX, 0);
}

int AudioSystem::stopHoldNote(int channel_offset) {
	return Mix_FadeOutChannel(HOLD_NOTE_FIRST_CHANNEL + channel_offset, HOLD_SFX_FADE_MS);
}

int AudioSystem::playDroppedNote() {
	return Mix_PlayChannel(-1, drop_SFX, 0);
}

int AudioSystem::playMissedNote() {
	return Mix_PlayChannel(-1, miss_SFX, 0);
}

int AudioSystem::playHitGood() {
	return Mix_PlayChannel(-1, hit_good_SFX, 0);
}

int AudioSystem::playHitPerfect() {
	return Mix_PlayChannel(-1, hit_perfect_SFX, 0);
}

int AudioSystem::playCountdownHigh() {
	return Mix_PlayChannel(-1, countdown_high_SFX, 0);
}

int AudioSystem::playCountdownLow() {
	return Mix_PlayChannel(-1, countdown_low_SFX, 0);
}

int AudioSystem::playApplause() {
	return Mix_PlayChannel(-1, applause_SFX, 0);
}

int AudioSystem::playModeChange() {
	return Mix_PlayChannel(-1, mode_change_SFX, 0);
}

int AudioSystem::playModeCountdown() {
	return Mix_PlayChannel(-1, mode_countdown_SFX, 0);
}


int AudioSystem::musicPlaying() {
	// return 1 if music still playing, 0 otherwise
	return Mix_PlayingMusic();
}

float AudioSystem::getSongPosition() {
	return Mix_GetMusicPosition(current_music);
}

float AudioSystem::getSongDuration() {
	return Mix_MusicDuration(current_music);
}

void AudioSystem::pauseMusic() {
	Mix_PauseMusic();
}

void AudioSystem::resumeMusic() {
	Mix_ResumeMusic();
}