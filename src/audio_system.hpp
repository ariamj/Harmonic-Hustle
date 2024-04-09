#pragma once
// Container for loading and playing sound files

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>

class AudioSystem 
{
public:
	AudioSystem();

	// Releases all associated resources
	~AudioSystem();

	bool init();

	int playOverworld();
	int playBattle(int enemy_id);
	int playLobby();

	int playHoldNote(int channel_offset);
	int stopHoldNote(int channel_offset);
	int playDroppedNote();
	int playMissedNote();

	int playHitGood();
	int playHitPerfect();

	int playCountdownLow();
	int playCountdownHigh();

	int playModeChange();
	int playModeCountdown();

	int playApplause();

	int musicPlaying();
	float getSongPosition();
	float getSongDuration();

	void pauseMusic();
	void resumeMusic();

private:

	const int HOLD_NOTE_FIRST_CHANNEL = 12;
	// How long it should take for held-note SFX to fade out
	const float HOLD_SFX_FADE_MS = 200.f;
	bool LoadFiles();

	Mix_Music* current_music;
	// music references
	Mix_Music* overworld_music;
	Mix_Music* lobby_music;
	std::vector<Mix_Music*> enemy_music;
	Mix_Chunk* drop_SFX;
	Mix_Chunk* hit_good_SFX;
	Mix_Chunk* hit_perfect_SFX;
	Mix_Chunk* miss_SFX;
	Mix_Chunk* hold_SFX;
	Mix_Chunk* countdown_high_SFX;
	Mix_Chunk* countdown_low_SFX;
	Mix_Chunk* applause_SFX;
	Mix_Chunk* mode_change_SFX;
	Mix_Chunk* mode_countdown_SFX;
};