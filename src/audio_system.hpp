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

	int playHoldNote(int channel_offset);
	int stopHoldNote(int channel_offset);
	int playDroppedNote();
	int playMissedNote();

	int playHitGood();
	int playHitPerfect();

	int musicPlaying();
	float getSongPosition();

	void pauseMusic();
	void resumeMusic();

private:

	// How long it should take for held-note SFX to fade out
	const float HOLD_SFX_FADE_MS = 200.f;
	bool LoadFiles();

	Mix_Music* current_music;
	// music references
	Mix_Music* overworld_music;
	std::vector<Mix_Music*> enemy_music;
	Mix_Chunk* drop_SFX;
	Mix_Chunk* hit_good_SFX;
	Mix_Chunk* hit_perfect_SFX;
	Mix_Chunk* miss_SFX;
	Mix_Chunk* hold_SFX;

};