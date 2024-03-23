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
#include "settings_screen.hpp"
#include "start_screen.hpp"
#include "audio_system.hpp"
#include "cutscene.hpp"
// #include "screen.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();
	
	Battle battle;
	Overworld overworld;
	Settings settings;
	Start start;
	AudioSystem audioSystem;
	Cutscene cutscene;
	
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
	//sets the current scene to overworld
	bool render_set_overworld_screen();
	
	// sets the current scene to battle
	bool render_set_battle_screen();

	// sets the current scene to settings/help
	bool render_set_settings_screen();

	// sets the current scene to start screen
	bool render_set_start_screen();

  // sets to cut scene
	bool render_set_cutscene();

	void checkEnemyPositions();

	vec2 getRamdomEnemyPosition();

	void handleEscInput(int action);
	void handleBackspaceInput(int action);
	void handleClickStartBtn();
	void handleClickHelpBtn();

	// Input callback functions
	void on_key(int key, int scancode, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mods);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	// Game state
	RenderSystem* renderer;
	float current_speed;
	// float next_enemy_spawn;
	Entity player_sprite;
	Entity battle_player_sprite;
	Entity battle_enemy_sprite;
	Entity judgement_line_sprite;

	// buttons
	// enum MouseArea {
	// 	in_unclickable,
	// 	in_start_btn,
	// 	in_help_btn,
	// };
	MouseArea mouse_area = in_unclickable;

	// music references
	// Mix_Music* background_music;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// Screen curr_scene;

};
