// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"

// Game configuration
const size_t MAX_ENEMIES = 2;
const size_t ENEMY_DELAY_MS = 5000 * 3;


// Create the bug world
WorldSystem::WorldSystem() :
	next_enemy_spawn(0.f) {
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

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// handle next step of game
	// Update window title with current scene
	std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Overworld";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step

	// Remove out of screen entities (Notes, etc.)
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if(!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// Spawn new enemies
	next_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
	if (registry.enemies.components.size() <= MAX_ENEMIES && next_enemy_spawn < 0.f) {
		// reset timer
		next_enemy_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_DELAY_MS / 2);
		// create an enemy
		createEnemy(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), window_height_px / 3));
	}

	// Process the player state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;

	return true;
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

	// Create a new Player
	player_sprite = createPlayer(renderer, { window_width_px/2, window_height_px/2 });
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// handle world collisions (if need?)
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// exit game or go back depending on game state
void handleEscInput(int action) {
	if (action == GLFW_PRESS) {

	}
}

// confirmation key
void handleEnterInput(int action) {
	if (action == GLFW_PRESS) {

	}
}

void handleMovementInput(int action, int key) {
	if (action == GLFW_PRESS) {
		std::cout << key << " PRESSED" << '\n';
	}
	else if (action == GLFW_RELEASE) {
		std::cout << key << " RELEASED" << '\n';
	}
}

void handleRhythmInput(int action, int key) {
	if (action == GLFW_PRESS) {

	}
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
    // handle key inputs
	switch (key) {
		case GLFW_KEY_ESCAPE:
			handleEscInput(action);
			break;
		case GLFW_KEY_ENTER:
			handleEnterInput(action);
			break;
		case GLFW_KEY_W:
		case GLFW_KEY_A:
		case GLFW_KEY_S:
			handleMovementInput(action, key);
			break;
		case GLFW_KEY_D:
			//if were in the overworld then
			handleMovementInput(action, key);
			//else 
			//handleRhythmInput(action, key);
			break;
		case GLFW_KEY_F:
		case GLFW_KEY_J:
		case GLFW_KEY_K:
			handleRhythmInput(action, key);
			break;
		default:
			break;
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
    // handle mouse inputs (if need?)
	(vec2)mouse_position; // dummy to avoid compiler warning
}
