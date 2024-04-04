// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"
#include <fstream>


// Create the bug world
WorldSystem::WorldSystem(){
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

}

WorldSystem::~WorldSystem() {
    // Destory all created components
	registry.clear_all_components();
	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}
int savefile_num = 0;
// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 1);

	// Create the main window (for rendering, keyboard, and mouse input)
	// Make main window size of the entire screen
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	
	gameInfo.width = mode->width;
	gameInfo.height = mode->height;

	window = glfwCreateWindow(mode->width, mode->height, "Harmonic Hustle", nullptr, nullptr);
	// window = glfwCreateWindow(window_width_px, window_height_px, "Harmonic Hustle", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_btn_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_btn_redirect);

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;

	// Serializer readerwriter = Serializer();

	gameInfo.lane_1 = gameInfo.width / 2 - 300;
	gameInfo.lane_2  = gameInfo.width / 2 - 100;
	gameInfo.lane_3 = gameInfo.width / 2 + 100;
	gameInfo.lane_4 = gameInfo.width / 2 + 300;

	gameInfo.curr_enemy = Entity{};

	overworld.init(window, renderer_arg, &saver);
	battle.init(window, renderer_arg, &audioSystem, &saver);
	settings.init(window, renderer_arg);
	start.init(window, renderer_arg);
	gameOver.init(window, renderer_arg);
	cutscene.init(window, renderer_arg);

	// Moved into here from main
	audioSystem.init();

	// Set all states to default
    restart_game();
}



// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {

	while (registry.texts.entities.size() > 0)
		registry.remove_all_components_of(registry.texts.entities.back());

	// add FPS to screen

	if (show_fps) {
		std::string fpsString = "FPS: " + std::to_string(FPS);
		createText(fpsString, vec2(gameInfo.width - 70.f, 20.f), 0.4f, Colour::white, glm::mat4(1.f), gameInfo.curr_screen, true);
	}

	if (gameInfo.curr_screen != Screen::START && gameInfo.curr_screen != Screen::SETTINGS && gameInfo.curr_screen != Screen::CUTSCENE) {
		// Help binding instruction
		vec3 text_colour = Colour::theme_blue_1;
		if (gameInfo.curr_screen == Screen::OVERWORLD) {
			text_colour = Colour::theme_blue_3;
		}
		createText("(?)[H]", vec2(10.f, 25.f), 0.75f, text_colour, glm::mat4(1.f), gameInfo.curr_screen, false);
	}

	if (gameInfo.curr_screen == Screen::OVERWORLD) {
		if (!gameInfo.is_intro_finished) {
			std::cout << "GO TO INTRO" << '\n';
			render_set_cutscene();
		}
		if (gameInfo.curr_level == gameInfo.max_level) {
			std::cout << "GO TO GAME OVER DIALOGUE" << '\n';
			render_set_cutscene();
		}
		return overworld.handle_step(elapsed_ms_since_last_update, current_speed);
	} else if (gameInfo.curr_screen == Screen::BATTLE) {
		bool song_playing = audioSystem.musicPlaying();
		bool toReturn = true;
		// once song ends, display battle over overlay
		// 		curr screen is set to overworld when battle is over and user presses SPACE
		if (!song_playing) {
			std::cout << "HEREEE" << std::endl;
			battle.handle_battle_end();
		} else {
			toReturn = battle.handle_step(elapsed_ms_since_last_update, current_speed);
		}
		if (gameInfo.curr_screen == Screen::OVERWORLD) {
			// go to boss cutscene
			if (gameInfo.curr_level == 4 && !gameInfo.is_boss_finished) {
				render_set_cutscene();
			}
			if (gameInfo.curr_level == 4 && gameInfo.gameIsOver) {
				render_set_cutscene();
			}
			render_set_overworld_screen();
		}
		return toReturn;
	} else if (gameInfo.curr_screen == Screen::SETTINGS) {
    	return settings.handle_step(elapsed_ms_since_last_update, current_speed);
	} else if (gameInfo.curr_screen == Screen::CUTSCENE) {
		// add in a restart here for after game over dialogue

		if (cutscene.game_over_dialogue_progress >= cutscene.GAME_OVER_DIALOGUE.size()) {
			std::cout << "RESTART GAME" << std::endl;
			gameInfo.is_game_over_finished = true;
			// overwrite prev save data since game is now finished	
			saver.save_game();
      render_set_game_over_screen();
		}
		else if (cutscene.boss_dialogue_progress >= cutscene.BOSS_DIALOGUE.size() && !gameInfo.gameIsOver) {
			std::cout << "GO TO BOSS BATTLE" << std::endl;
			gameInfo.is_boss_finished = true;
			battle.start();
			render_set_battle_screen();
			return battle.handle_step(elapsed_ms_since_last_update, current_speed);
		}
		else if ((cutscene.intro_dialogue_progress >= cutscene.INTRO_DIALOGUE.size()) && !gameInfo.is_intro_finished) {
			gameInfo.is_intro_finished = true;
			render_set_overworld_screen();
		}
		return cutscene.handle_step(elapsed_ms_since_last_update, current_speed);
	} else if (gameInfo.curr_screen == Screen::START) {
		return start.handle_step(elapsed_ms_since_last_update, current_speed);
	} else {
		return gameOver.handle_step(elapsed_ms_since_last_update, current_speed);
	}
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
    // handle restarting game (if need?)
	// Debugging for memory/component leaks

	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created
	// All that have a motion (maybe exclude player? !!!<TODO>)
	while (registry.motions.entities.size() > 0)
	    registry.remove_all_components_of(registry.motions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();

	// Title
	// TODO: put onto some sort of splash screen
	// createText("~ HARMONIC HUSTLE ~", vec2((gameInfo.width/2.f), gameInfo.height * 5.f/6.f), 2.0f, glm::vec3(1.0, 0.0, 1.0), glm::mat4(1.f), Screen::OVERWORLD);

	// Create a new Player
	player_sprite = createPlayer(renderer, { gameInfo.width/2, gameInfo.height/2 });
	gameInfo.player_sprite = std::make_shared<Entity>(player_sprite);
	
	// create set number of enemies
	// createEnemy(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), window_height_px / 3), 1);
	createEnemy(renderer, getRamdomEnemyPosition(), 1);
	createEnemy(renderer, getRamdomEnemyPosition(), 1);
	createEnemy(renderer, getRamdomEnemyPosition(), 1);

	createEnemy(renderer, getRamdomEnemyPosition(), 2);
	createEnemy(renderer, getRamdomEnemyPosition(), 2);
	createEnemy(renderer, getRamdomEnemyPosition(), 2);

	createEnemy(renderer, getRamdomEnemyPosition(), 3);
	createEnemy(renderer, getRamdomEnemyPosition(), 3);
	createEnemy(renderer, getRamdomEnemyPosition(), 3);
	

	checkEnemyPositions();

	// pause enemies for a sec on game start
	if (!registry.pauseEnemyTimers.has(player_sprite)) {
		registry.pauseEnemyTimers.emplace(player_sprite);
	}

	// createText("~ BATTLE TIME ~", vec2((gameInfo.width/2.f), (gameInfo.height/8.f)), 2.0f, glm::vec3(1.0, 0.0, 1.0), glm::mat4(1.f), Screen::BATTLE);

	float xDisplacement = PORTRAIT_WIDTH * 3.f / 7.f;
	float yDisplacement = PORTRAIT_HEIGHT / 2;

	battle_player_sprite = createBattlePlayer(renderer, { xDisplacement + 20.f, yDisplacement + 20.f });
    battle_enemy_sprite = createBattleEnemy(renderer, { gameInfo.width - yDisplacement - 20.f, gameInfo.height - xDisplacement - 20.f });

	registry.battleEnemy.emplace(battle_enemy_sprite);
	registry.battlePlayer.emplace(battle_player_sprite);

	// hard coded values for now
	float judgement_line_y_pos = gameInfo.height / 1.2;
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_1, judgement_line_y_pos });
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_2, judgement_line_y_pos });
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_3, judgement_line_y_pos });
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_4, judgement_line_y_pos });
	
	// reset cut scene info
	gameInfo.gameIsOver = false;
	cutscene.boss_dialogue_progress = 0;
	cutscene.intro_dialogue_progress = 0;
	cutscene.game_over_dialogue_progress = 0;
	gameInfo.is_boss_finished = false;
	gameInfo.is_game_over_finished = false;
	gameInfo.is_intro_finished = false;

	gameInfo.curr_level = 1;

	start.init_screen();
	gameOver.init_screen();

	// set current screen to start screen on every restart
	// render_set_overworld_screen();
	// render_set_battle_screen();
	render_set_start_screen();
	// render_set_game_over_screen();
}

// Compute collisions between entities
// no collision handling for settings
void WorldSystem::handle_collisions() {
	// handle world collisions (if need?)
	switch(gameInfo.curr_screen) {
		case Screen::OVERWORLD:
			if (overworld.handle_collisions()) {
				// transition to game over screen if collide with higher lvl enemy
				restart_game();
				render_set_game_over_screen();
			} else if (gameInfo.curr_screen == Screen::BATTLE) {
				render_set_battle_screen();
				battle.start();
			}
			break;
		default:
			battle.handle_collisions();
			break;
	}
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// REQUIRES current scene NOT be overworld
// switch to overworld scene
bool WorldSystem::render_set_overworld_screen() {
	Screen prevScreen = gameInfo.curr_screen;
	gameInfo.curr_screen = Screen::OVERWORLD;
	start.set_visible(false);
	gameOver.set_visible(false);
	settings.set_visible(false);
	battle.set_visible(false);
	cutscene.set_visible(false);
	overworld.set_visible(true);
	// don't restart the overworld music if coming from settings or start screen
	if (prevScreen != Screen::SETTINGS && prevScreen != Screen::START)
		audioSystem.playOverworld();
	std::cout << "current screen: overworld" << std::endl;
	return true; // added to prevent error
};

// REQUIRES current scene NOT be battle
// switch to battle scene
bool WorldSystem::render_set_battle_screen() {
	gameInfo.curr_screen = Screen::BATTLE;
	start.set_visible(false);
	gameOver.set_visible(false);
	settings.set_visible(false);
	overworld.set_visible(false);
	cutscene.set_visible(false);
	battle.set_visible(true);
	// sets the player velocity to 0 once screen switches
	if (registry.motions.has(player_sprite)) {
		registry.motions.get(player_sprite).velocity = {0, 0};
	}
	// add a timer so every time it switches enemies pause for a bit
	if (!registry.pauseEnemyTimers.has(player_sprite)) {
		registry.pauseEnemyTimers.emplace(player_sprite);
	}
	std::cout << "current screen: battle" << std::endl;
	return true; // added to prevent error
};

// REQUIRES current scene to NOT be settings
// switch to battle scene
bool WorldSystem::render_set_settings_screen() {
	gameInfo.curr_screen = Screen::SETTINGS;
	overworld.set_visible(false);
	battle.set_visible(false);
	start.set_visible(false);
	gameOver.set_visible(false);
	cutscene.set_visible(false);
	settings.set_visible(true);

	// sets the player velocity to 0 once screen switches
	if (registry.motions.has(player_sprite)) {
		registry.motions.get(player_sprite).velocity = {0, 0};
	}

	// add a timer so every time it switches enemies pause for a bit
	if (!registry.pauseEnemyTimers.has(player_sprite)) {
		registry.pauseEnemyTimers.emplace(player_sprite);
	}

	std::cout << "current screen: settings" << std::endl;
	return true; // added to prevent error
};

// REQUIRES current scene to NOT be start screen
// switch to start screen
bool WorldSystem::render_set_start_screen() {
	Screen prevScreen = gameInfo.curr_screen;
	gameInfo.curr_screen = Screen::START;
	overworld.set_visible(false);
	battle.set_visible(false);
	settings.set_visible(false);
  	cutscene.set_visible(false);
	gameOver.set_visible(false);
	start.set_visible(true);

	// sets the player velocity to 0 once screen switches
	if (registry.motions.has(player_sprite)) {
		registry.motions.get(player_sprite).velocity = {0, 0};
	}
  
  	// don't restart the music if coming from help screen
	if (prevScreen != Screen::SETTINGS)
		audioSystem.playOverworld();

	std::cout << "current screen: start" << std::endl;
  	return true; // added to prevent error
}

// REQUIRES current scene to NOT be game over screen
// switch to game over screen
bool WorldSystem::render_set_game_over_screen() {
	Screen prevScreen = gameInfo.curr_screen;
	gameInfo.curr_screen = Screen::GAMEOVER;
	overworld.set_visible(false);
	battle.set_visible(false);
	settings.set_visible(false);
  	cutscene.set_visible(false);
	start.set_visible(false);
	gameOver.set_visible(true);

	// sets the player velocity to 0 once screen switches
	if (registry.motions.has(player_sprite)) {
		registry.motions.get(player_sprite).velocity = {0, 0};
	}
  
  	// don't restart the music if coming from help screen
	if (prevScreen != Screen::SETTINGS)
		audioSystem.playOverworld();

	std::cout << "current screen: game over" << std::endl;
  	return true; // added to prevent error
}

// REQUIRES current scene NOT be cutscene
// switch to cutscene
bool WorldSystem::render_set_cutscene() {
	gameInfo.curr_screen = Screen::CUTSCENE;
	start.set_visible(false);
	gameOver.set_visible(false);
	settings.set_visible(false);
	overworld.set_visible(false);
	battle.set_visible(false);
	cutscene.set_visible(true);
  
	// sets the player velocity to 0 once screen switches
	if (registry.motions.has(player_sprite)) {
		registry.motions.get(player_sprite).velocity = { 0, 0 };
	}
	// add a timer so every time it switches enemies pause for a bit
	if (!registry.pauseEnemyTimers.has(player_sprite)) {
		registry.pauseEnemyTimers.emplace(player_sprite);
	}

	cutscene.remove_prev_assets();

	std::cout << "current screen: cutscene" << std::endl;
	return true; // added to prevent error
};

void WorldSystem::checkEnemyPositions() {
	vec2 playerPos = registry.motions.get(player_sprite).position;
	// float xPadding = PLAYER_WIDTH / 2.f;
	// float doubleXPadding = xPadding * 2.f;
	// float yPadding = PLAYER_HEIGHT / 2.f;
	// float doubleYPadding = yPadding * 2.f;
	
	auto& enemies = registry.enemies.entities;
	for (Entity enemy : enemies) {
		Motion& enemyMotion = registry.motions.get(enemy);
		float xDis = playerPos.x - enemyMotion.position.x;
		float yDis = playerPos.y - enemyMotion.position.y;
		float distance = xDis * xDis + yDis * yDis;
		distance = sqrt(distance);
		vec2 newPos = {0, 0};
		while (distance < PLAYER_ENEMY_RADIUS ) {
			newPos = getRamdomEnemyPosition();
			xDis = playerPos.x - newPos.x;
			yDis = playerPos.y - newPos.y;
			distance = xDis * xDis + yDis * yDis;
			distance = sqrt(distance);
		}
		if (newPos.x != 0 && newPos.y != 0) {
			enemyMotion.position = newPos;
		}
	}
};

vec2 WorldSystem::getRamdomEnemyPosition() {
	std::default_random_engine rng = std::default_random_engine(std::random_device()());
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	float xPadding = ENEMY_WIDTH / 2.f;
	float doubleXPadding = xPadding * 2.f;
	float yPadding = ENEMY_HEIGHT / 2.f;
	float doubleYPadding = yPadding * 2.f;

	return {xPadding + uniform_dist(rng) * (gameInfo.width - doubleXPadding),
			yPadding + uniform_dist(rng) * (gameInfo.height - doubleYPadding)};
};

// For testing/debugging purposes
void testButton(Entity& btn) {
	// test button clicks
	if (registry.colours.has(btn)) {
		vec3 colour = registry.colours.get(btn);
		registry.colours.remove(btn);
		if (colour == Colour::theme_blue_2) {
			registry.colours.insert(btn, Colour::dark_blue);
		} else {
			registry.colours.insert(btn, Colour::theme_blue_2);
		}
	}
}

// exit game key
// 	-> on exit, saves game and closes window
void WorldSystem::handleEscInput(int action) {
	if (action == GLFW_PRESS) {
		std::cout << "esc press" << std::endl;
		//only save if exit in overworld
		if (gameInfo.curr_screen == Screen::OVERWORLD) {
			saver.save_game();
		}
		glfwSetWindowShouldClose(window, 1);
	}
}

// help key
void WorldSystem::handleHInput(int action) {
	if (action == GLFW_PRESS) {
		std::cout << "H key press" << std::endl;

		// if curr screen is overworld, switch to settings
		if (gameInfo.curr_screen == Screen::OVERWORLD) {
			gameInfo.prev_screen = Screen::OVERWORLD;
			render_set_settings_screen();

		// if curr screen is battle, pause the battle and then switch to settings
		} else if (gameInfo.curr_screen == Screen::BATTLE) {
			gameInfo.prev_screen = Screen::BATTLE;
			battle.set_pause(true);
			render_set_settings_screen();

		} else if (gameInfo.curr_screen == Screen::START) {
			gameInfo.prev_screen = Screen::START;
			render_set_settings_screen();
		} else if (gameInfo.curr_screen == Screen::GAMEOVER) {
			gameInfo.prev_screen = Screen::GAMEOVER;
			render_set_settings_screen();
		} else if (gameInfo.curr_screen == Screen::SETTINGS) {
			// if curr screen is settings,
			//		-> prev screen is overworld, just switch back
			//      -> prev screen is battle, update screen visabilities and SET PAUSE for battle NOT set visible
			//				(set visible restarts the battle, set pause just unpausees it)
			if (gameInfo.prev_screen == Screen::OVERWORLD) {
				render_set_overworld_screen();
			} else if (gameInfo.prev_screen == Screen::BATTLE) {
				gameInfo.curr_screen = Screen::BATTLE;
				settings.set_visible(false);
				overworld.set_visible(false);
				cutscene.set_visible(false);
				battle.set_pause(false);
				std::cout << "current screen: battle" << std::endl;
			} else if (gameInfo.prev_screen == Screen::START) {
				render_set_start_screen();
			} else if (gameInfo.prev_screen == Screen::GAMEOVER) {
				render_set_game_over_screen();
			}

		}
		// disable help/settings during cutscenes
		// else if (gameInfo.curr_screen == Screen::CUTSCENE) {
		// 	battle.start();
		// 	render_set_battle_screen();
		// }
	}
}

// confirmation key
void handleEnterInput(int action) {
	if (action == GLFW_PRESS) {
		std::cout << "enter press" << std::endl;
	}
}

// temp - testing start screen
void WorldSystem::handleBackspaceInput(int action) {
	if (action == GLFW_PRESS) {
		std::cout << "space pressed" << std::endl;
		if (gameInfo.curr_screen == Screen::OVERWORLD || gameInfo.curr_screen == Screen::SETTINGS) {
			render_set_start_screen();
		}
	}
}

void WorldSystem::handleClickStartBtn() {
	std::cout << "Clicked on 'START'" << std::endl;
	// test button clicks
	// testButton(start.start_btn);

	// To overworld
	if (gameInfo.curr_screen == Screen::START) {
		render_set_overworld_screen();
	}
}

void WorldSystem::handleClickHelpBtn() {
	std::cout << "Clicked on 'HELP'" << std::endl;
	// test button clicks
	// testButton(start.help_btn);

	// To settings
	if (gameInfo.curr_screen == Screen::START || gameInfo.curr_screen == Screen::GAMEOVER) {
		gameInfo.prev_screen = gameInfo.curr_screen;
		render_set_settings_screen();
	}
}

void WorldSystem::handleClickRestartBtn() {
	std::cout << "Clicked on 'RESTART" << std::endl;

	// To Start
	if (gameInfo.curr_screen == Screen::GAMEOVER) {
		restart_game();
		render_set_start_screen();
	}
}

void WorldSystem::handleClickLoadBtn() {
	// Load button is only located on start screen
	if (gameInfo.curr_screen != Screen::START) {
		return;
	}

	Serializer readerwriter = Serializer();
	gameInfo.existing_enemy_info.clear();
	// change this to load game on a button in start screen
	if (readerwriter.load_game()) {
		// remove initial enemies
		for (auto e : registry.enemies.entities) {
			registry.remove_all_components_of(e);
		}
		// repopulate with enemies from save file
		for (auto& enemy : gameInfo.existing_enemy_info) {
			createEnemy(renderer, vec2(enemy[0], enemy[1]), enemy[2]);
		}
	}

	if (gameInfo.gameIsOver) {
		restart_game();
	}
	// if there are no enemies and game is not over, switch to boss battle
	else if (gameInfo.existing_enemy_info.size() == 0) {
		render_set_cutscene();
	}

	// To overworld
	if (gameInfo.curr_screen == Screen::START) {
		render_set_overworld_screen();
	}
}	

// On key callback
void WorldSystem::on_key(int key, int scancode, int action, int mod) {

	// global keys:
	// esc -> exit/pause
	// enter -> 
	
	// overworld keys:
	// WASD -> walking

	// battle keys:
	// DFJK -> rhythm

	switch (key) {
		case GLFW_KEY_C:			
			// hard code scene switching to key 'c' for now
			if (action == GLFW_PRESS) {
				switch (gameInfo.curr_screen) {
					case Screen::OVERWORLD:
						render_set_battle_screen();
						battle.start();
						break;
					case Screen::BATTLE:
						// pressing 'C' during battle immedidately ends battle, for testing
						// added guard to prevent pressing when battle is already over
						if (!battle.battle_is_over) {
							// battle.score = 9999; // auto-win
							battle.handle_battle_end();
						}
						break;
					default:
						break;
				};
			}
			
			break;
		case GLFW_KEY_V:
			// for testing purposes
			if (action == GLFW_PRESS) {
				render_set_cutscene();
			}
			break;
		case GLFW_KEY_TAB:
			// toggle show FPS on TAB key
			if (action == GLFW_PRESS) {
				show_fps = !show_fps;
			}
			break;
		case GLFW_KEY_ESCAPE:
			handleEscInput(action);
			break;
		case GLFW_KEY_H: 
			handleHInput(action);
			break;
		case GLFW_KEY_ENTER:
			handleEnterInput(action);
			break;
		case GLFW_KEY_BACKSPACE:
			handleBackspaceInput(action);
			break;
		default:
			if (gameInfo.curr_screen == Screen::OVERWORLD) {
				overworld.handle_key(key, scancode, action, mod);
			} else if (gameInfo.curr_screen == Screen::BATTLE) {
				if (action == GLFW_PRESS) {
					battle.handle_key(key, scancode, action, mod);
				}
				if (gameInfo.curr_screen == Screen::OVERWORLD) {
					render_set_overworld_screen();
				}
			} else if (gameInfo.curr_screen == Screen::START) {
				start.handle_key(key, scancode, action, mod);
				if (gameInfo.curr_screen == Screen::OVERWORLD) {
					render_set_overworld_screen();
				}
			} else if (gameInfo.curr_screen == Screen::GAMEOVER) {
				gameOver.handle_key(key, scancode, action, mod);
				if (gameInfo.curr_screen == Screen::START) {
					render_set_start_screen();
				}
			}
			else if (gameInfo.curr_screen == Screen::CUTSCENE) {
				cutscene.handle_key(key, scancode, action, mod);
			}
			break;
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	double xpos = mouse_position.x;
    double ypos = mouse_position.y;

	float y_padding = 50.f; // for some reason y coords a bit off...

	/* Start screen buttons*/
	if (gameInfo.curr_screen == Screen::START) {
		// START button
		BoxAreaBound start_btn_area = registry.boxAreaBounds.get(start.start_btn);
		bool within_start_btn_area = (xpos >= start_btn_area.left) && (xpos <= start_btn_area.right) && (ypos >= start_btn_area.top - y_padding) && (ypos <= start_btn_area.bottom - y_padding);
		// HELP button
		BoxAreaBound help_btn_area = registry.boxAreaBounds.get(start.help_btn);
		bool within_help_btn_area = (xpos >= help_btn_area.left) && (xpos <= help_btn_area.right) && (ypos >= help_btn_area.top - y_padding) && (ypos <= help_btn_area.bottom - y_padding);
		
		// LOAD button
		BoxAreaBound load_btn_area = registry.boxAreaBounds.get(start.load_from_save_btn);
		bool within_load_btn_area = (xpos >= load_btn_area.left) && (xpos <= load_btn_area.right) && (ypos >= load_btn_area.top - y_padding) && (ypos <= load_btn_area.bottom - y_padding);
		
		if (within_start_btn_area) {
			// std::cout << "in start button area" << std::endl;
			mouse_area = in_start_btn;
		} else if (within_help_btn_area) {
			// std::cout << "in help button area" << std::endl;
			mouse_area = in_help_btn;
		} else if (within_load_btn_area) {
			mouse_area = in_load_btn;
		} else {
			mouse_area = in_unclickable;
		}
		start.handle_mouse_move(mouse_area);
	}

	/* Game Over screen buttons*/
	if (gameInfo.curr_screen == Screen::GAMEOVER) {
		// START button
		BoxAreaBound restart_btn_area = registry.boxAreaBounds.get(gameOver.restart_btn);
		bool within_restart_btn_area = (xpos >= restart_btn_area.left) && (xpos <= restart_btn_area.right) && (ypos >= restart_btn_area.top - y_padding) && (ypos <= restart_btn_area.bottom - y_padding);
		// HELP button
		BoxAreaBound help_btn_area = registry.boxAreaBounds.get(gameOver.help_btn);
		bool within_help_btn_area = (xpos >= help_btn_area.left) && (xpos <= help_btn_area.right) && (ypos >= help_btn_area.top - y_padding) && (ypos <= help_btn_area.bottom - y_padding);
		if (within_restart_btn_area) {
			// std::cout << "in start button area" << std::endl;
			mouse_area = in_restart_btn;
		} else if (within_help_btn_area) {
			// std::cout << "in help button area" << std::endl;
			mouse_area = in_help_btn;
		} else {
			mouse_area = in_unclickable;
		}
		gameOver.handle_mouse_move(mouse_area);
	}
}

void WorldSystem::on_mouse_button(int button, int action, int mods) {
	// handle mouse button events	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		switch(mouse_area) {
			case in_start_btn:
				handleClickStartBtn();
				break;
			case in_help_btn:
				handleClickHelpBtn();
				break;
			case in_restart_btn:
				handleClickRestartBtn();
        break;
			case in_load_btn:
				handleClickLoadBtn();
				break;
			default:
				std::cout << "not in clickable area" << std::endl;
				break;
		}
	}
}
