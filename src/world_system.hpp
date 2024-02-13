#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "overworld_screen.hpp"
#include "battle_screen.hpp"
#include "screen.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	Overworld overworld;
	Battle battle;

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;
private:
	// Input callback functions
	void on_key(int key, int scancode, int action, int mod);
	void on_mouse_move(vec2 pos);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	// Game state
	RenderSystem* renderer;

	// music references
	Mix_Music* background_music;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// bool is_in_battle;
	// bool is_in_overworld;
	Screen curr_scene;
	// TODO for future -> add menu screen bool?

};
