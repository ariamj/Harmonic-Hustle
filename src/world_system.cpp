// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>
#include "physics_system.hpp"

// Game configuration


// Create the bug world
WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	Mix_CloseAudio();

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

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());

	if (background_music == nullptr) { // add "%s\n" for each sound added
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// handle next step of game
	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
    // handle restarting game (if need?)
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
