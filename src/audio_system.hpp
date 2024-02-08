#pragma once
// Container for loading and playing sound files

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
};