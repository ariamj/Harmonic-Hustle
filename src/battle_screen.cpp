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
const vec3 ALRIGHT_COLOUR = { 255.f, 255.f, 1.f };
const vec3 MISSED_COLOUR = { 255.f, 1.f, 1.f };

// the time it should take for note to fall from top to bottom
const float NOTE_TRAVEL_TIME = 2000.f;

// rhythmic input timing variables, initialized in .init
float spawn_offset; 
// TODO: Use BattleInfo structs instead. These are hard-coded to match enemy0.wav
int num_notes = 32;
float note_spawns[32];
int next_note_index = 1;
float bpm = 130.f;
float bpm_ratio = bpm / 60.f;

// Enemy-specific battle information
// TODO: Load information into these, instead hard-coding as above
const int num_unique_battles = 2;
BattleInfo battleInfo[num_unique_battles];

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

	// Used to spawn notes relative to judgment line instead of window height
	spawn_offset = -(NOTE_TRAVEL_TIME - (NOTE_TRAVEL_TIME * (1 - 1.f / 1.25f)));

	// OPTIMIZE: Read these from a file instead
	std::vector<float> note_timings = {4.f, 5.f, 6.f, 6.5f, 7.f, 
								12.f, 13.f, 14.f, 14.5f, 15.f,
								20.f, 21.f, 22.f, 22.5f, 23.f,
								28.f, 29.f, 30.f, 30.5f, 31.f,
								40.f, 41.f, 42.f, 43.f, 44.f, 45.5f,
								56.f, 57.f, 58.f, 59.f, 60.f, 61.5f};
	

	// Convert beat-based timings into seconds-based timings, in ms
	for (int i = 0; i < note_timings.size(); i++) {
		note_spawns[i] = (1000.f * note_timings[i] / bpm_ratio) + spawn_offset;
		// std::cout << note_spawns[i] << "\n";
	}

	// TODO: Account for when first note spawn is negative (before music starts)
	next_note_spawn = note_spawns[0];
};

bool Battle::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Battle";
	title_ss << " --- FPS: " << FPS;
	title_ss << " --- Score: " << score; // TEMP !!! TODO: render score on screen
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Remove out of screen entities (Notes, etc.)

	if (battle_is_over) {
		
	} else {
		auto& motions_registry = registry.motions;

		// Process the player state
		assert(registry.screenStates.components.size() <= 1);
		ScreenState &screen = registry.screenStates.components[0];

		float min_counter_ms = 3000.f;
		next_note_spawn -= elapsed_ms_since_last_update;

		if (registry.notes.components.size() < MAX_NOTES && next_note_spawn < 0.f) {
			// set next timer, subtracting the "overshot" time (next_note_spawn <= 0.f) during this frame
			next_note_spawn = note_spawns[next_note_index] - note_spawns[next_note_index - 1] + next_note_spawn;
			if (next_note_index <= num_notes) {
				// spawn notes in the four lanes
				createNote(renderer, vec2(lanes[rand() % 4], 0.f));
			}
			next_note_index += 1;
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
				audio.playDroppedNote();
				standing = missed;
				score += standing;
				registry.remove_all_components_of(motions_registry.entities[i]);
			}
		}
	} 

		// update notes positions
		for (int i = 0; i < registry.motions.components.size(); ++i) {
			Motion& motion = registry.motions.components[i];

		if (registry.notes.has(motions_registry.entities[i])) {
			// Increment progress on range [0,1]
			float progress_step = elapsed_ms_since_last_update / NOTE_TRAVEL_TIME;
			motion.progress = min(1.f, motion.progress + progress_step);

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
	}

	return true;
};

bool Battle::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Battle scene back
	// 		remove battle over popups as needed
	//		reset note spawns as needed
    if (is_visible) {
		registry.screens.insert(curr_screen_entity, Screen::BATTLE);
		setBattleIsOver(false);
		auto& notes = registry.notes.entities;
		for (Entity note : notes) {
			registry.renderRequests.remove(note);
			registry.notes.remove(note);
		}
	}

    return is_visible;
};

// if battle is over, render pop up parts
// 		else reset it by removing pop up parts
void Battle::setBattleIsOver(bool isOver) {
	// std::cout << "test set battle is over: " << isOver << std::endl;
	battle_is_over = isOver;
	auto& popUpEntities = registry.battleOverPopUpParts.entities;
	if (battle_is_over) {
		for (Entity popUpEntity : popUpEntities) {
			if (!registry.screens.has(popUpEntity)) {
				RenderRequest rr = registry.renderRequests.get(popUpEntity);
				registry.renderRequests.remove(popUpEntity);
				registry.renderRequests.insert(popUpEntity, rr);
				registry.screens.insert(popUpEntity, Screen::BATTLE );
			}
		}
	} else {
		for (Entity popUpEntity : popUpEntities) {
			if (registry.screens.has(popUpEntity)) {
				registry.screens.remove(popUpEntity);
			}
		}
	}
}

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
				Motion lane_motion = registry.motions.get(entity);
				float lane = lane_motion.position.x;
				// Key - Judgment line collision checker:
				if ((d_key_pressed && lane == gameInfo.lane_1) || (f_key_pressed && lane == gameInfo.lane_2) 
						|| (j_key_pressed && lane == gameInfo.lane_3) || (k_key_pressed && lane == gameInfo.lane_4)) {
					got_hit = 1; // did not miss the note

					// Determine score standing
					// Simple standing feedback using judgement line colour
					vec3& colour = registry.colours.get(entity);
					JudgementLine judgement_line = registry.judgmentLine.get(entity);
					float note_y_pos = registry.motions.get(entity_other).position.y;
					float lane_y_pos = lane_motion.position.y;
					float judgement_line_half_height = lane_motion.scale.y * judgement_line.actual_img_scale_factor;
					float scoring_margin = 3.f;
					if ((note_y_pos < lane_y_pos - judgement_line_half_height) || (note_y_pos > lane_y_pos + judgement_line_half_height)) {
						// set standing to Alright
						standing = alright;
						colour = ALRIGHT_COLOUR;
					} else if (((note_y_pos >= lane_y_pos - judgement_line_half_height) && (note_y_pos < lane_y_pos - scoring_margin))
								|| ((note_y_pos > lane_y_pos + scoring_margin) && (note_y_pos <= lane_y_pos + judgement_line_half_height))) {
						// set standing to Good
						standing = good;
						colour = GOOD_COLOUR;
					} else if ((note_y_pos >= lane_y_pos - scoring_margin) && (note_y_pos <= lane_y_pos + scoring_margin)) {
						// set standing to Perfect
						standing = perfect;
						colour = PERFECT_COLOUR;
					}
					score += standing;

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
			audio.playMissedNote(); // placeholder sound effect
		}
	}
	registry.collisions.clear();
	key_pressed = false;
	d_key_pressed = false;
	f_key_pressed = false;
	j_key_pressed = false;
	k_key_pressed = false;
	
	
	// set battle is over as needed
	// 		if battle is over, add all battle over pop up parts to battle screen
	// setting it to true for now
	// setBattleIsOver(true);
	
};

// battle keys:
// DFJK -> rhythm
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
						colour = MISSED_COLOUR;
					}
			}
		}
	}
}

void handleDebug(int action) {
	if (action == GLFW_PRESS) {
		debugging.in_debug_mode = !debugging.in_debug_mode;
	}
}

void Battle::handle_key(int key, int scancode, int action, int mod) {
	if (battle_is_over) {
		switch(key) {
			case GLFW_KEY_T:
				if (action == GLFW_PRESS) { 
					setBattleIsOver(!battle_is_over);
					std::cout << "test toggle battle is over: " << battle_is_over << std::endl;
				}
				break;
			case GLFW_KEY_SPACE:
				if (action == GLFW_PRESS) { 
					gameInfo.curr_screen = Screen::OVERWORLD;
					std::cout << "test return to overworld after battle " << battle_is_over << std::endl;
				}
				break;
			default:
				std::cout << "unhandled key" << std::endl;
				break;
		}
	} else {
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
			case GLFW_KEY_X:
				handleDebug(action);
				break;
			case GLFW_KEY_T:
				if (action == GLFW_PRESS) { 
					
					setBattleIsOver(!battle_is_over);
					std::cout << "test toggle battle is over: " << battle_is_over << std::endl;
				}
				break;
			default:
				std::cout << "unhandled key" << std::endl;
				break;
		}
	}
};

void Battle::handle_mouse_move(vec2 pos) {
    
};

// From https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/a-brief-introduction-to-lerp-r4954/
// Linked on Canvas M1 requirements
float Battle::lerp(float start, float end, float t) {
	return start * (1-t) + end * t;
}