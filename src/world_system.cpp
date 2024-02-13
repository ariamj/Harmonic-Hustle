// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"
#include "overworld_screen.hpp"
#include "battle_screen.hpp"

// Game configuration


// Create the bug world
WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

	// set current scene to overworld
	curr_scene = Screen::OVERWORLD;
	// curr_scene = Screen::BATTLE;

	overworld.init();
	battle.init();

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
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, "Harmonic Hustle", nullptr, nullptr);
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
	if (curr_scene == Screen::OVERWORLD) {
		overworld.handle_step(elapsed_ms_since_last_update);
	} else if (curr_scene == Screen::BATTLE) {
		battle.handle_step(elapsed_ms_since_last_update);
	}
	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
    // handle restarting game (if need?)
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// handle world collisions (if need?)
	if (curr_scene == Screen::OVERWORLD) {
		overworld.handle_collisions();
	} else if (curr_scene == Screen::BATTLE) {
		battle.handle_collisions();
	}
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// exit game or go back depending on game state
void handleEscInput(int action) {
	if (action == GLFW_PRESS) {
		std::cout << "esc press" << std::endl;
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
				if (curr_scene == Screen::OVERWORLD) {
					curr_scene = Screen::BATTLE;
					std::cout << "current screen: battle" << std::endl;
				} else {
					curr_scene = Screen::OVERWORLD;
					std::cout << "current screen: overworld" << std::endl;
				}
			}
			break;
		case GLFW_KEY_ESCAPE:
			handleEscInput(action);
			break;
		case GLFW_KEY_ENTER:
			handleEnterInput(action);
			break;
		default:
			if (curr_scene == Screen::OVERWORLD) {
				overworld.handle_key(key, scancode, action, mod);
			} else if (curr_scene == Screen::BATTLE) {
				battle.handle_key(key, scancode, action, mod);
			}
			break;
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
    // handle mouse inputs (if need?)
	(vec2)mouse_position; // dummy to avoid compiler warning
}
