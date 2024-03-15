// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"

// Create the bug world
WorldSystem::WorldSystem(){
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

}

WorldSystem::~WorldSystem() {
    // Destory all created components

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

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
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;

	gameInfo.lane_1 = gameInfo.width / 2 - 300;
	gameInfo.lane_2  = gameInfo.width / 2 - 100;
	gameInfo.lane_3 = gameInfo.width / 2 + 100;
	gameInfo.lane_4 = gameInfo.width / 2 + 300;

	gameInfo.curr_enemy = Entity{};

	overworld.init(window, renderer_arg);
	battle.init(window, renderer_arg, &audioSystem);
	settings.init(window, renderer_arg);

	// Moved into here from main
	audioSystem.init();

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {

	if (gameInfo.curr_screen == Screen::OVERWORLD) {
		return overworld.handle_step(elapsed_ms_since_last_update, current_speed);
	} else if (gameInfo.curr_screen == Screen::BATTLE) {
		bool song_playing = audioSystem.musicPlaying();
		bool toReturn = true;
		// once song ends, display battle over overlay
		// 		curr screen is set to overworld when battle is over and user presses SPACE
		if (!song_playing) {
			battle.handle_battle_end();
		} else {
			toReturn = battle.handle_step(elapsed_ms_since_last_update, current_speed);
		}
		if (gameInfo.curr_screen == Screen::OVERWORLD) {
			render_set_overworld_screen();
		}
		return toReturn;
	} else {
		return settings.handle_step(elapsed_ms_since_last_update, current_speed);
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
	createText("~ HARMONIC HUSTLE ~", vec2((gameInfo.width/2.f), gameInfo.height * 5.f/6.f), 2.0f, glm::vec3(1.0, 0.0, 1.0), glm::mat4(1.f), Screen::OVERWORLD);

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

	createText("~ BATTLE TIME ~", vec2((gameInfo.width/2.f), (gameInfo.height/8.f)), 2.0f, glm::vec3(1.0, 0.0, 1.0), glm::mat4(1.f), Screen::BATTLE);

	float xDisplacement = PORTRAIT_WIDTH * 3.f / 7.f;
	float yDisplacement = PORTRAIT_HEIGHT / 2;

	battle_player_sprite = createBattlePlayer(renderer, { xDisplacement, yDisplacement});
    battle_enemy_sprite = createBattleEnemy(renderer, { gameInfo.width - yDisplacement, gameInfo.height - xDisplacement });

	// hard coded values for now
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_1, gameInfo.height / 1.2 });
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_2, gameInfo.height / 1.2 });
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_3, gameInfo.height / 1.2 });
	judgement_line_sprite = createJudgementLine(renderer, { gameInfo.lane_4, gameInfo.height / 1.2 });

	// set current screen to overworld on every restart
	render_set_overworld_screen();
	// render_set_battle_screen();
}

// Compute collisions between entities
// no collision handling for settings
void WorldSystem::handle_collisions() {
	// handle world collisions (if need?)
	switch(gameInfo.curr_screen) {
		case Screen::OVERWORLD:
			if (overworld.handle_collisions()) {
				restart_game();
			} else if (gameInfo.curr_screen == Screen::BATTLE) {
				battle.start();
				render_set_battle_screen();
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
	settings.set_visible(false);
	battle.set_visible(false);
	overworld.set_visible(true);
	// don't restart the overworld music if coming from settings
	if (prevScreen != Screen::SETTINGS)
		audioSystem.playOverworld();
	std::cout << "current screen: overworld" << std::endl;
	return true; // added to prevent error
};

// REQUIRES current scene NOT be battle
// switch to battle scene
bool WorldSystem::render_set_battle_screen() {
	gameInfo.curr_screen = Screen::BATTLE;
	settings.set_visible(false);
	overworld.set_visible(false);
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

void WorldSystem::checkEnemyPositions() {
	vec2 playerPos = registry.motions.get(player_sprite).position;
	float xPadding = PLAYER_WIDTH / 2.f;
	float doubleXPadding = xPadding * 2.f;
	float yPadding = PLAYER_HEIGHT / 2.f;
	float doubleYPadding = yPadding * 2.f;
	
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

// open pause menu or go back depending on game state
// 		-> currently toggle help screen will only work in OVERWORLD
//		TODO -> add in pause for battle?
void WorldSystem::handleEscInput(int action) {
	if (action == GLFW_PRESS) {
		std::cout << "esc press" << std::endl;
		
		// glfwSetWindowShouldClose(window, 1);
		if (gameInfo.curr_screen == Screen::OVERWORLD) {
			render_set_settings_screen();
		} else if (gameInfo.curr_screen == Screen::SETTINGS) {
			render_set_overworld_screen();
		}
	}
}

// confirmation key
void handleEnterInput(int action) {
	if (action == GLFW_PRESS) {
		std::cout << "enter press" << std::endl;
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
						battle.start();
						render_set_battle_screen();
						break;
					case Screen::BATTLE:
					default:
						// pressing 'C' during battle immedidately ends battle, for testing
						// added guard to prevent pressing when battle is already over
						if (!battle.battle_is_over) {
							battle.handle_battle_end();
						}

						// render_set_overworld_screen();
						break;
				};
			}
			
			break;
		case GLFW_KEY_ESCAPE:
			handleEscInput(action);
			break;
		case GLFW_KEY_ENTER:
			handleEnterInput(action);
			break;
		default:
			if (gameInfo.curr_screen == Screen::OVERWORLD) {
				overworld.handle_key(key, scancode, action, mod);
			} else if (gameInfo.curr_screen == Screen::BATTLE) {
				battle.handle_key(key, scancode, action, mod);
				if (gameInfo.curr_screen == Screen::OVERWORLD) {
					render_set_overworld_screen();
				}
			}
			break;
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
    // handle mouse inputs (if need?)
	(vec2)mouse_position; // dummy to avoid compiler warning
}
