#include "battle_screen.hpp"
// #include "world_init.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include "world_init.hpp"
#include <math.h>
#include "tiny_ecs_registry.hpp"

Battle::Battle() {
    rng = std::default_random_engine(std::random_device()());
}

Battle::~Battle() {

};

void Battle::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
};

bool Battle::handle_step(float elapsed_ms_since_last_update) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Battle";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step

	// Remove out of screen entities (Notes, etc.)
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	// for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	//     Motion& motion = motions_registry.components[i];
	// 	if (motion.position.x + abs(motion.scale.x) < 0.f) {
	// 		if(!registry.players.has(motions_registry.entities[i])) // don't remove the player
	// 			registry.remove_all_components_of(motions_registry.entities[i]);
	// 	}
	// }

	// Process the player state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;

    return true;
};

bool Battle::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    // TODO add more checks
    if (is_visible) {
        if (registry.screens.has(registry.screenStates.entities[0])) registry.screens.remove(registry.screenStates.entities[0]);
        registry.screens.insert(registry.screenStates.entities[0], Screen::BATTLE);
    } else {
        registry.screens.remove(registry.screenStates.entities[0]);
    }
    return is_visible;
};

void Battle::handle_collisions() {

};

void handleRhythmInput(int action, int key) {
	if (action == GLFW_PRESS) {
        std::cout << "rhythm input: " << key << std::endl;
	}
}

void Battle::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_D:
		case GLFW_KEY_F:
		case GLFW_KEY_J:
		case GLFW_KEY_K:
            handleRhythmInput(action, key);
            break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
};

void Battle::handle_mouse_move(vec2 pos) {
    
};

void Battle::handle_reset() {
    // registry.list_all_components();
	// printf("Restarting Battle\n");

	// // Reset the game speed
	// current_speed = 1.f;

	// // Remove all entities that we created
	// // All that have a motion (maybe exclude player? !!!<TODO>)
	// while (registry.motions.entities.size() > 0)
	//     registry.remove_all_components_of(registry.motions.entities.back());

	// // Debugging for memory/component leaks
	// registry.list_all_components();

	// // Create a new Battle Player
	// player_battle_sprite = createBattlePlayer(renderer, { 200.f, 200.f});

    // enemy_battle_sprite = createBattleEnemy(renderer, { window_width_px - 200.f, window_height_px - 200.f });
};