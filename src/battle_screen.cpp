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
const size_t NOTE_SPAWN_DELAY = 2000;
const vec3 PERFECT_COLOUR = { 255.f, 1.f, 255.f };
const vec3 GOOD_COLOUR = { 1.f, 255.f, 1.f };
const vec3 MISSED_COLOUR = { 255.f, 1.f, 1.f };

// lanes where notes will spawn
// float lanes[4] = { LANE_1, LANE_2, LANE_3, LANE_4 };

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
    lanes[0] = gameInfo.lane_1;
    lanes[1] = gameInfo.lane_2;
    lanes[2] = gameInfo.lane_3;
    lanes[3] = gameInfo.lane_4;
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
		createNote(renderer, vec2(lanes[rand() % 4], gameInfo.height / 10));
	}

	// Remove entities that leave the screen below
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
		Motion& motion = motions_registry.components[i];
		if (motion.position.y + abs(motion.scale.y) > gameInfo.height+50.f) {
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
			// Increment progress on range [0,1]
			motion.progress = min(1.f, motion.progress + NOTE_POSITION_STEP_SIZE);

			// Interpolate note position from top to bottom of screen
			motion.position.y = lerp(0.0, float(gameInfo.height), motion.progress);

			// Interpolate note size, increasing from top (1x) to bottom (2.5x) of screen
			motion.scale_factor = lerp(1.0, 2.5, motion.progress); 
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

	// judgement line timers
	min_counter_ms = 200.f;
	for (Entity line : registry.judgmentLineTimers.entities) {
		if (registry.judgmentLineTimers.has(line)) {
			// progress timer
			JudgementLineTimer& counter = registry.judgmentLineTimers.get(line);
			counter.count_ms -= elapsed_ms_since_last_update;
			if (counter.count_ms < min_counter_ms) {
				min_counter_ms = counter.count_ms;
			}
			// change judgement line colour back after timer expires
			if (counter.count_ms < 0) {
				vec3& colour = registry.colours.get(line);
				colour = {1.f, 1.f, 1.f};
				registry.judgmentLineTimers.remove(line);
			}
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
			if (registry.judgmentLine.has(entity) && registry.notes.has(entity_other)) {
				float lane = registry.motions.get(entity).position.x;
				// Key - Judgment line collision checker:
				if ((d_key_pressed && lane == gameInfo.lane_1) || (f_key_pressed && lane == gameInfo.lane_2) 
						|| (j_key_pressed && lane == gameInfo.lane_3) || (k_key_pressed && lane == gameInfo.lane_4)) {
					got_hit = 1; // did not miss the note
					// change node colour on collision
					//vec3& colour = registry.colours.get(entity);		// uncomment these two lines if want node colour change
					//colour = PERFECT_COLOUR;							// but can't press more than one key at the same time for input
					registry.collisionTimers.emplace(entity_other);
					registry.remove_all_components_of(entity_other);	// comment this line out if want node colour change
				}
			}
		}
		if (got_hit) {
			// TODO MUSIC: play sound for successful hit note
			audio.playHitPerfect();
		}
		else {
			// TODO MUSIC: add correct sound
			audio.playMissedNote(); // placeholder sound effect
		}
	}
	registry.collisions.clear();
	key_pressed = false;
	d_key_pressed = false;
	f_key_pressed = false;
	j_key_pressed = false;
	k_key_pressed = false;
	
	
	
};

// battle keys:
// DFJK -> rhythm
// currently if any of DFJK is pressed, will check for collision
// TODO: Split into DFJK-specific column detection

void handleRhythmInput(int action, int key) {
	auto& collisionsRegistry = registry.collisions;
	auto& collisionsTimerRegistry = registry.collisionTimers;
	if (action == GLFW_PRESS) {
        std::cout << "rhythm input: " << key << std::endl;
		if (key == GLFW_KEY_D || key == GLFW_KEY_F || key == GLFW_KEY_J || key == GLFW_KEY_K) {
			key_pressed = true;
			// Change judgment line colour on input
			ComponentContainer<Motion> motion_container = registry.motions;
			for (Entity line : registry.judgmentLine.entities) {
				float lane = motion_container.get(line).position.x;
				if ((key == GLFW_KEY_D && lane == gameInfo.lane_1)
					|| (key == GLFW_KEY_F && lane == gameInfo.lane_2)
					|| (key == GLFW_KEY_J && lane == gameInfo.lane_3)
					|| (key == GLFW_KEY_K && lane == gameInfo.lane_4)) {
						// change corresponding judgement line colour
						registry.judgmentLineTimers.emplace_with_duplicates(line);
						vec3& colour = registry.colours.get(line);
						colour = GOOD_COLOUR;
					}
			}
		}
	}
}

void Battle::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_D:
			d_key_pressed = true;
			handleRhythmInput(action, key);
            break;
		case GLFW_KEY_F:
			f_key_pressed = true;
			handleRhythmInput(action, key);
            break;
		case GLFW_KEY_J:
			j_key_pressed = true;
			handleRhythmInput(action, key);
            break;
		case GLFW_KEY_K:
			k_key_pressed = true;
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