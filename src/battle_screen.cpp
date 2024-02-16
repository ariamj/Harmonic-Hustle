#include "battle_screen.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <audio_system.hpp>

// consts for now;
const size_t MAX_NOTES = 10;
const size_t NOTE_SPAWN_DELAY = 3000;

// lanes where notes will spawn
float lanes[4] = { LANE_1, LANE_2, LANE_3, LANE_4 };

AudioSystem audio = AudioSystem();


Battle::Battle() {
    rng = std::default_random_engine(std::random_device()());
}

Battle::~Battle() {

};

void Battle::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
	audio.init();
};

bool Battle::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Battle";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove out of screen entities (Notes, etc.)
	auto& motions_registry = registry.motions;

	// Process the player state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;
	next_note_spawn -= elapsed_ms_since_last_update * current_speed;

	if (registry.notes.components.size() < MAX_NOTES && next_note_spawn < 0.f) {
		// reset timer
		next_note_spawn = (NOTE_SPAWN_DELAY / 2) + uniform_dist(rng) * (NOTE_SPAWN_DELAY / 2);
		// spawn notes in the four lanes
		createNote(renderer, vec2(lanes[rand() % 4], window_height_px / 10));
	}

	// Remove entities that leave the screen below
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
		Motion& motion = motions_registry.components[i];
		if (motion.position.y + abs(motion.scale.y) > window_height_px+50.f) {
			// remove missed notes and play missed note sound
			// TODO MUSIC: replace chicken dead sound
			if (registry.notes.has(motions_registry.entities[i])) {
				audio.playMissedNote();
				registry.remove_all_components_of(motions_registry.entities[i]);
			}
		}
	}

	 // update notes positions
	for (int i = 0; i < registry.motions.components.size(); ++i) {
		Motion& motion = registry.motions.components[i];

		if (registry.notes.has(motions_registry.entities[i])) {
			// motion.position.y += motion.velocity.y * elapsed_ms_since_last_update / 1000.0f;
			// Replaced direct addition with interpolation instead of using velocity
			// 
			motion.progress = min(1.f, motion.progress + NOTE_POSITION_STEP_SIZE);
			motion.position.y = lerp(0.0, float(window_height_px), motion.progress);

			// TODO: Interpolate one other property of a note

			// Increasing note size over time is buggy, unless we change get_bounding_box in physics_system.cpp
			// motion.scale.x = lerp(NOTE_WIDTH, 3.f * NOTE_WIDTH, motion.progress);
			// motion.scale.y = lerp(NOTE_HEIGHT, 3.f * NOTE_HEIGHT, motion.progress);
		}
	}

	// collision timers
	for (Entity entity : registry.collisionTimers.entities) {
		CollisionTimer& ct = registry.collisionTimers.get(entity);
		ct.counter_ms -= elapsed_ms_since_last_update;

		if (ct.counter_ms < 0) {
			registry.collisions.remove(entity);
			registry.collisionTimers.remove(entity);
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

bool key_pressed = false;
//TODO: change color of note and play event sound
void Battle::handle_collisions() {
	int got_hit = 0; // 0 if didn't hit any notes, 1 otherwise
	if (key_pressed) {
		// Loop over all collisions detected by the physics system
		auto& collisionsRegistry = registry.collisions;
		for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
			// The entity and its collider
			Entity entity = collisionsRegistry.entities[i];
			Entity entity_other = collisionsRegistry.components[i].other;

			// check if judgment line
			if (registry.judgmentLine.has(entity)) {
				got_hit = 1; // did not miss the note
				// Key - Judgment line collision checker:
				if (registry.notes.has(entity_other)) {
					registry.collisionTimers.emplace(entity_other);
					registry.remove_all_components_of(entity_other);
				}

			}
		}
		if (got_hit) {
			// TODO MUSIC: play sound for successful hit note
		}
		else {
			// TODO MUSIC: add correct sound
			audio.playMissedNote(); // placeholder sound effect
		}
	}
	registry.collisions.clear();
	key_pressed = false;
	
	
	
};

// battle keys:
// DFJK -> rhythm
// currently if any of DFJK is pressed, will check for collision

void handleRhythmInput(int action, int key) {
	auto& collisionsRegistry = registry.collisions;
	auto& collisionsTimerRegistry = registry.collisionTimers;
	if (action == GLFW_PRESS) {
        std::cout << "rhythm input: " << key << std::endl;
		if (key == GLFW_KEY_D || key == GLFW_KEY_F || key == GLFW_KEY_J || key == GLFW_KEY_K) {
			key_pressed = true;
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

// From https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/a-brief-introduction-to-lerp-r4954/
// Linked on Canvas M1 requirements
float Battle::lerp(float start, float end, float t) {
	return start * (1-t) + end * t;
}