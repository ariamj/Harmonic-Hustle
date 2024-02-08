#include <common.hpp>
#include <audio_system.hpp>
#include <iostream>

AudioSystem::AudioSystem() {
	return;
}


AudioSystem::~AudioSystem() {
	return;
}

bool AudioSystem::init() {
	LoadFiles();
	return true;
}

bool AudioSystem::LoadFiles() {
	return true; // Successfully loaded all files
}

bool AudioSystem::playBGM() {
	return true;
}