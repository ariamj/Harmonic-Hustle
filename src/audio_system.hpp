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

	bool playOverworld();
	bool playBattle(int enemy_id);
	bool playMissedNote();

private:
	bool LoadFiles();

	// music references
	Mix_Music* overworld_music;
	std::vector<Mix_Music*> enemy_music;
	Mix_Chunk* missed_note_music;

};