#include "battle_screen.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// consts for now;
const size_t MAX_NOTES = 5;
const size_t NOTE_SPAWN_DELAY = 2000;

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

bool Battle::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Battle";
	glfwSetWindowTitle(window, title_ss.str().c_str());
	//float current_speed = 1.0f; // speed at which note falls

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
	next_note_spawn -= elapsed_ms_since_last_update * current_speed;

	if (registry.enemies.components.size() < MAX_NOTES && next_note_spawn < 0.f) {
		// reset timer
		next_note_spawn = (NOTE_SPAWN_DELAY / 2) + uniform_dist(rng) * (NOTE_SPAWN_DELAY / 2);
		// create an enemy

		createEnemy(renderer, vec2(150.f + uniform_dist(rng) * (window_width_px - 500.f), window_height_px / 10),1);
	}

	//if (registry.notes.components.size() <= MAX_NOTES && next_note_spawn < 0.f) {

	//	// reset timer
	//	next_note_spawn = (NOTE_SPAWN_DELAY / 2) + uniform_dist(rng) * (NOTE_SPAWN_DELAY / 2);
	//	// create an enemy

	//	//createNote(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), window_height_px / 3));
	//}


	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
		Motion& motion = motions_registry.components[i];
		if (motion.position.y + abs(motion.scale.y) > 1400.f) {
			registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	 // update npc positions
	for (int i = 0; i < registry.motions.components.size(); ++i) {
		Motion& motion = registry.motions.components[i];

		if (registry.enemies.has(motions_registry.entities[i])) {

			motion.position.y += motion.velocity.y * elapsed_ms_since_last_update / 1000.0f;
		}
	}




    return true;
};

bool Battle::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Battle scene back
    if (is_visible) registry.screens.insert(curr_screen_entity, Screen::BATTLE);

    return is_visible;
};

// change color of note and play event sound
void Battle::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// check if judgment line
		if (registry.judgmentLine.has(entity)) {
			
			// Key - Judgment line collision checker:
			//if (registry.notes.has(entity_other)) {
			//	registry.collisionTimers.emplace(entity);
			//	// now if key is pressed during timer, then it will disappear. otherwise turn red as it leaves off screen.
			//}

			if (registry.notes.has(entity_other)) {
				registry.collisionTimers.emplace(entity);
				// now if key is pressed during timer, then it will disappear. otherwise turn red as it leaves off screen.
			}
		}
	}
	registry.collisions.clear();
	
};

// battle keys:
// DFJK -> rhythm
// currently if any of DFJK is pressed, will check for collision

void handleRhythmInput(int action, int key) {
	if (action == GLFW_PRESS) {
        std::cout << "rhythm input: " << key << std::endl;
		if (key == GLFW_KEY_D || key == GLFW_KEY_F || key == GLFW_KEY_J || key == GLFW_KEY_K) {
			// handle collision or miss
			
			
		}
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