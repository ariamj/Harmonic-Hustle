#include "overworld_screen.hpp"
// #include "world_init.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// Game configuration
const size_t MAX_ENEMIES = 2;
const size_t ENEMY_DELAY_MS = 5000 * 3;

Overworld::Overworld() 
: next_enemy_spawn(0.f) 
{
    rng = std::default_random_engine(std::random_device()());
}

Overworld::~Overworld() {

};

void Overworld::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
};

bool Overworld::handle_step(float elapsed_ms_since_last_update) {
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
};

bool Overworld::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    return is_visible;
};

void Overworld::handle_collisions() {

};

void handleMovementInput(int action, int key) {
	if (action == GLFW_PRESS) {
		std::cout << key << " OVERWORLD PRESSED" << '\n';
	}
	else if (action == GLFW_RELEASE) {
		std::cout << key << " OVERWORLD RELEASED" << '\n';
	}
}

void Overworld::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_W:
		case GLFW_KEY_A:
		case GLFW_KEY_S:
		case GLFW_KEY_D:
            handleMovementInput(action, key);
			break;
        default:
            std::cout << "unhandled overworld key" << std::endl;
            break;
    }
};

void Overworld::handle_mouse_move(vec2 pos) {
    
};

void Overworld::handle_reset() {
    // registry.list_all_components();
	// printf("Restarting\n");

	// // Reset the game speed
	// current_speed = 1.f;

	// // Remove all entities that we created
	// // All that have a motion (maybe exclude player? !!!<TODO>)
	// while (registry.motions.entities.size() > 0)
	//     registry.remove_all_components_of(registry.motions.entities.back());

	// // Debugging for memory/component leaks
	// registry.list_all_components();

	// // Create a new Player
	// player_sprite = createPlayer(renderer, { window_width_px/2, window_height_px/2 });
};