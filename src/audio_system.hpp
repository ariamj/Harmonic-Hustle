#pragma once
// Container for loading and playing sound files

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class AudioSystem 
{
public:
	AudioSystem();

	// Releases all associated resources
	~AudioSystem();

	bool init();

	bool playBGM();
private:
	bool LoadFiles();

	// music references
	Mix_Music* background_music;

};