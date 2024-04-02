#include "battle_screen.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <audio_system.hpp>
#include "chrono"
#include <algorithm>

using Clock = std::chrono::high_resolution_clock;


int count_late = 0;
int count_early = 0;
float total_late_distance = 0;
float total_early_distance = 0;

// DEBUG MEMORY LEAKS (WINDOWS ONLY)
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/find-memory-leaks-using-the-crt-library?view=msvc-170
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//_CrtMemState s1;
//_CrtMemState s2;


Serializer s = Serializer();

Battle::Battle() {
    rng = std::default_random_engine(std::random_device()());
}

Battle::~Battle() {

};


bool Battle::handle_step(float elapsed_ms_since_last_update, float current_speed) {
	std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Battle";
	// TODO: render score on screen instead
	title_ss << " --- Score: " << score;
	if (debugging.in_debug_mode) {
		// TODO: render threshold on screen instead
		title_ss << " --- Score Threshold: " << score_threshold;
	}
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Remove out of screen entities (Notes, etc.)
	// while (registry.texts.entities.size() > 0)
	// 	registry.remove_all_components_of(registry.texts.entities.back());
	
	// render judgement line key prompts
	float text_y_pos = gameInfo.height/1.2f + 100.f;
	vec3 text_colour = Colour::light_gray;
	float text_scale = 1.5f;
	createText("D", vec2(gameInfo.lane_1, text_y_pos), text_scale, text_colour, glm::mat4(1.f), Screen::BATTLE, true);
	createText("F", vec2(gameInfo.lane_2, text_y_pos), text_scale, text_colour, glm::mat4(1.f), Screen::BATTLE, true);
	createText("J", vec2(gameInfo.lane_3, text_y_pos), text_scale, text_colour, glm::mat4(1.f), Screen::BATTLE, true);
	createText("K", vec2(gameInfo.lane_4, text_y_pos), text_scale, text_colour, glm::mat4(1.f), Screen::BATTLE, true);

	if (registry.combos.components.size() == 0) {
		Entity c = createText("Combo: " + std::to_string(combo), vec2(gameInfo.width/2.f, 20.f), 1.f, text_colour, glm::mat4(1.f), Screen::BATTLE, true);
		registry.combos.emplace(c);
	}

	if (in_countdown) {
		// if in countdown mode, update the timer
		//		if countdown is done, resume the game
		//		else render the countdown number on screen
		countdownTimer_ms -= elapsed_ms_since_last_update;

		if (countdownTimer_ms <= 0) {
			// fully resume game
			in_countdown = false;
			audio->resumeMusic();
		} else {
			// render count down text
			int time = (int) (countdownTimer_ms / 1000) + 1;
			createText(std::to_string(time), vec2(gameInfo.width / 2.f, gameInfo.height / 2.f), 3.5f, Colour::white, glm::mat4(1.f), Screen::BATTLE, true);
		}
	} else if (battle_is_over) {
		//TODO render in text that has:
		//		battle outcome, player score and a "press space to continue" line
		float spacing = 50.f;
		// Get sizing of battle over overlay
		Motion overlay_motion = registry.motions.get(gameOverPopUpOverlay);
		float score_x_spacing = overlay_motion.scale.x/8.f;
		if (battleWon()) {
			createText("Congratulations!!!", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 4)), 0.75f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
			createText("Enemy has been defeated", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 3)), 0.75f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
		} else {
			createText("You have been defeated!!!", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 3.5)), 0.75f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
		}
		createText("Score: " + std::to_string((int)score), vec2(gameInfo.width/2.f - (score_x_spacing * 2), gameInfo.height/2.f - spacing), 0.75f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
		createText("Enemy: " + std::to_string((int)score_threshold), vec2(gameInfo.width/2.f + (score_x_spacing * 2), gameInfo.height/2.f - spacing), 0.75f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
		
		// Scoring
		float scoring_text_size = 0.5f;
		createText("Perfect", vec2(gameInfo.width/2.f -( score_x_spacing * 3), gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_purple, glm::mat4(1.f), Screen::BATTLE, true);
		createText("Good", vec2(gameInfo.width/2.f - score_x_spacing, gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_green, glm::mat4(1.f), Screen::BATTLE, true);
		createText("Alright", vec2(gameInfo.width/2.f + score_x_spacing, gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_yellow, glm::mat4(1.f), Screen::BATTLE, true);
		createText("Missed", vec2(gameInfo.width/2.f + (score_x_spacing * 3), gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_red, glm::mat4(1.f), Screen::BATTLE, true);

		createText(std::to_string(perfect_counter), vec2(gameInfo.width/2.f -( score_x_spacing * 3), gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_purple, glm::mat4(1.f), Screen::BATTLE, true);
		createText(std::to_string(good_counter), vec2(gameInfo.width/2.f - score_x_spacing, gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_green, glm::mat4(1.f), Screen::BATTLE, true);
		createText(std::to_string(alright_counter), vec2(gameInfo.width/2.f + score_x_spacing, gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_yellow, glm::mat4(1.f), Screen::BATTLE, true);
		createText(std::to_string(missed_counter), vec2(gameInfo.width/2.f + (score_x_spacing * 3), gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_red, glm::mat4(1.f), Screen::BATTLE, true);

		// Combo
		createText("Best Combo: " + std::to_string(max_combo), vec2(gameInfo.width / 2.f, gameInfo.height / 2.f + (spacing * 4)), 1.f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

		// next instruction
		createText("Press space to continue", vec2(gameInfo.width/2.f, gameInfo.height/2.f + (spacing * 6)), 0.4f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

		// notify savenext_note_spawn
		createText("Game saved", vec2(gameInfo.width / 2.f, gameInfo.height / 2.f - (spacing * 6)), 0.4f, Colour::green, glm::mat4(1.f), Screen::BATTLE, true);
	} else {
		auto& motions_registry = registry.motions;

		float adjusted_elapsed_time = elapsed_ms_since_last_update;
		// Update song position 
		float new_song_position = audio->getSongPosition() * 1000.f - conductor.offset;
		// Check if song position has udpated (it doesn't update every frame)
		if (new_song_position > conductor.song_position) {
			adjusted_elapsed_time = new_song_position - conductor.song_position;
			conductor.song_position = new_song_position;
		}
		// Guard against negative (due to metadata offset) to prevent elapsed time from taking over
		else if (new_song_position > 0.f) {
			// Use elapsed-time when consecutive queries return same value
			conductor.song_position += elapsed_ms_since_last_update;
		}


		// TODO (?): Initiate some visual FX on every beat of song
		// Track each beat of song 
		if (conductor.song_position > last_beat + conductor.crotchet) {
			 // std::cout << "Beat detected" << "\n";
			 last_beat += conductor.crotchet;
		}

		// Spawning notes based on song position
		if (next_note_index < num_notes) {
			// Peek ahead to spawn multiple notes
			int multiple_note_index = min(next_note_index + NUM_LANES - 1, num_notes);

			// Create a small vector of available columns to spawn notes in
			std::vector<int> lane_indices;
			for (int i = 0; i < NUM_LANES; i++) {
				// Disallow spawning notes during held note durations
				if (lane_locked[i] < 0.f) {
					lane_indices.push_back(i);
				}
			}

			// Randomly shuffle lane indices
			std::shuffle(lane_indices.begin(), lane_indices.end(), random_generator);

			// Spawn in order of shuffled lane indices
				// With held notes, there may no longer be a lane available
				// This should NOT happen if the beatmap is well-designed
				// If notes spawn suddenly in the middle of screen, then there is an error in beatmap design
			for (int k = 0; k < lane_indices.size(); k++) {
				NoteInfo note = battleInfo[enemy_index].notes[next_note_index];
				if (conductor.song_position >= note.spawn_time) {
					createNote(renderer, vec2(lanes[lane_indices[k]], 0.f), note.spawn_time, note.duration);
					next_note_index += 1;
					// Set duration 
					lane_locked[lane_indices[k]] = note.duration + conductor.crotchet / 4.f;
				}
				if (next_note_index >= multiple_note_index) {
					break;
				}
			}
		}

		// Decrease durations of held lanes
		for (int i = 0; i < NUM_LANES; i++) {
			// Prevent underflow
			lane_locked[i] = max(lane_locked[i] - adjusted_elapsed_time, NO_DURATION);
		}

		// Remove entities that leave the screen below
		// Iterate backwards to be able to remove without unterfering with the next object to visit
		// (the containers exchange the last element with the current)
		for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
			Motion& motion = motions_registry.components[i];
			if (motion.position.y + abs(motion.scale.y) > gameInfo.height+50.f) {
				// remove missed notes and play missed note sound
				if (registry.notes.has(motions_registry.entities[i])) {
					audio->playDroppedNote();
					standing = missed;
					missed_counter++;
					score += standing;
					registry.remove_all_components_of(motions_registry.entities[i]);
				}
			}
		} 

		// Update note positions based on conductor time
		for (auto entity : registry.notes.entities) {
			if (!registry.motions.has(entity)) {
				continue;
			}
			Note& note = registry.notes.get(entity);
			Motion& motion = registry.motions.get(entity);

			float progress = (conductor.song_position - note.spawn_time + gameInfo.frames_adjustment * APPROX_FRAME_DURATION) / note_travel_time;
			motion.position.y = lerp(0.0, float(gameInfo.height), progress);
			motion.scale_factor = lerp(1.0, NOTE_MAX_SCALE_FACTOR, progress);	
		}

		// Update battle mode based on conductor time
		if (mode_index < battleInfo[enemy_index].modes.size()) {
			float mode_change_time = battleInfo[enemy_index].modes[mode_index].first;
			if (conductor.song_position >= mode_change_time) {
				switch (battleInfo[enemy_index].modes[mode_index].second) {
				case back_and_forth:
					gameInfo.battleModeColor = { -1.f, 0.2f, 1.f, 0.f }; // no adjust
					break;
				case beat_rush:
					gameInfo.battleModeColor = { 1.5f, -0.2f, -0.2f, -0.2f }; // adjust to red
					break;
				case unison:
					gameInfo.battleModeColor = { 1.f, -0.2f, -1.f, 0.f }; // adjust to orange
					break;
				}
				mode_index += 1;
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
		float min_counter_ms = 200.f;
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

		for (int i = 0; i < NUM_LANES; i++) {
			if (lane_hold[i] > 0.f) {
				// Player successfully held for full duration
				lane_hold[i] -= adjusted_elapsed_time;
				if (lane_hold[i] < 0.f) {
					audio->playHitPerfect();
				}
			}
		}
	}
	return true;
};

// when battle ends, 
// 		set battle is over to true, TODO -> render text on screen
//  	if won, remove all same level entities from screen, increment player lvl
//			increment player level
//		if lost, remove only collided with enemy on screen
void Battle::handle_battle_end() {
	// replay current lvl battle music for the battle over popup
	audio->playBattle(enemy_index);

	// Calculate a recommended calibration for timing for player
	// float adjustment = calculate_adjustment();

	// Only process events once
	if (battle_is_over) {
		return;
	}
	setBattleIsOver(true);

	// Delete any remaining note entities
	for (auto entity : registry.notes.entities) {
		registry.remove_all_components_of(entity);
	}
	// Delete any remaining sparks entities
	for (auto entity : registry.particleEffects.entities) {
		registry.remove_all_components_of(entity);
	}
	// Reset judgment line colours
	for (auto entity : registry.judgmentLine.entities) {
		vec3& colour = registry.colours.get(entity);
		colour = vec3(1.f);
	}

	// Remove particle generators
	renderer->particle_generators.clear();

	// battle won
	if (battleWon()) {
		// remove all lower lvl enemies
		int currLevel = gameInfo.curr_level;
		auto& enemies = registry.enemies.entities;
		for (Entity enemy : enemies) {
			int currEnemyLevel = registry.levels.get(enemy).level;
			if (currEnemyLevel == currLevel) {
				registry.enemies.remove(enemy);
				registry.renderRequests.remove(enemy);
			}
		}

		// increment player lvl
		gameInfo.curr_level = min(gameInfo.curr_level + 1, gameInfo.max_level);
		registry.levels.get(*gameInfo.player_sprite).level = gameInfo.curr_level;
		
	// battle lost
	} else {
		// remove colllided with enemy (give player another chance)
		registry.enemies.remove(gameInfo.curr_enemy);
		registry.renderRequests.remove(gameInfo.curr_enemy);
	}
	std::cout <<"SAVING AFTER BATTLE" << std::endl;
	saver->save_game();
	gameInfo.curr_enemy = {};
}

void Battle::start() {
	// DEBUG MEMORY LEAKS (WINDOWS ONLY)
	 //_CrtMemCheckpoint(&s1);

	// Local variables to improve readability
	enemy_index = min(gameInfo.curr_level - 1, NUM_UNIQUE_BATTLES - 1); // -1 for 0-indexing
	num_notes = battleInfo[enemy_index].count_notes;
	
	// Set Conductor variables
	conductor.bpm = battleInfo[enemy_index].bpm;
	conductor.crotchet = 60.f / battleInfo[enemy_index].bpm * 1000.f;
	conductor.offset = battleInfo[enemy_index].metadata_offset;
	conductor.song_position = 0.f;
	last_beat = 0.f; // moving reference point

	// Reset score
	score = 0;
	// Reset score threshold
	score_threshold = ceil(num_notes * perfect / 2);

	// Reset counters
	perfect_counter = 0;
    good_counter = 0;
    alright_counter = 0;
    missed_counter = 0;

	combo = 0;
	max_combo = 0;

	// TODO (?): Account for when note spawns are negative (before music starts)
	next_note_index = 0;
	mode_index = 0;

	// Fine-tuning timing
	count_late = 0;
	count_early = 0;
	total_late_distance = 0;
	total_early_distance = 0;

	Entity e = registry.battleEnemy.entities[0];
	RenderRequest& render = registry.renderRequests.get(e);

	switch (gameInfo.curr_level) {
		case 1:
			render.used_texture = TEXTURE_ASSET_ID::BATTLEENEMY;
			break;
		case 2:
			render.used_texture = TEXTURE_ASSET_ID::BATTLEENEMY_DRUM;
			break;
		case 3:
			render.used_texture = TEXTURE_ASSET_ID::BATTLEENEMY_MIC;
			break;
		case 4:
			render.used_texture = TEXTURE_ASSET_ID::BATTLEBOSS;
			gameInfo.gameIsOver = true;
			break;
		default:
			std::cout << "game level too high" << "\n";
	}

	// Create generators for particles that appear in the battle scene
	renderer->createParticleGenerator((int)PARTICLE_TYPE_ID::TRAIL);
	renderer->createParticleGenerator((int)PARTICLE_TYPE_ID::SPARK);

	audio->playBattle(enemy_index); // switch to battle music
	setBattleIsOver(false);
}

// if isPaused = true, pause music
// if isPaused = false, set to in countdown mode and reset timer
//		-> requires prev screen to NOT be battle if resuming
bool Battle::set_pause(bool isPaused) {
	if (isPaused) {
		audio->pauseMusic();
	} else {
		in_countdown = true;
		countdownTimer_ms = 3000;

		// double prevPosition = audio->getSongPosition();
		// double newPosition = prevPosition - 3.0;
		// audio->resumeMusic(newPosition);
		this->is_visible = true;

		Entity& curr_screen_entity = registry.screenStates.entities[0];
		// remove curr screen component
		if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);
		
		registry.screens.insert(curr_screen_entity, Screen::BATTLE);
	}
	return true; //silence warning
}

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
	if (battle_is_over) {
		vec2 center = {gameInfo.width / 2.f, gameInfo.height / 2.f};
		// the lighter box on top
		gameOverPopUpOverlay = createBox(center, {gameInfo.width / 2.f - 20.f, gameInfo.height / 2.f - 20.f});
		// the bigger border box
		Entity gameOverPopUp = createBox(center, {gameInfo.width / 2.f, gameInfo.height / 2.f});

		registry.colours.insert(gameOverPopUpOverlay, Colour::theme_blue_1);
		// registry.colours.insert(gameOverPopUpOverlay, {0.048, 0.184, 0.201});
		registry.colours.insert(gameOverPopUp, Colour::theme_blue_2);

		registry.battleOverPopUpParts.emplace(gameOverPopUpOverlay);
		registry.battleOverPopUpParts.emplace(gameOverPopUp);

		registry.screens.insert(gameOverPopUpOverlay, Screen::BATTLE);
		registry.screens.insert(gameOverPopUp, Screen::BATTLE);
	} else {
		while (registry.battleOverPopUpParts.entities.size() > 0)
			registry.remove_all_components_of(registry.battleOverPopUpParts.entities.back());
	}
}

// Return true if won battle (ie. points greater than needed threshold)
bool Battle::battleWon() {
	return (score > score_threshold);
}

bool key_pressed = false;
void Battle::handle_collisions() {
	// Variables to store hits
	std::vector<Entity> lane1_hits;
	std::vector<Entity> lane2_hits;
	std::vector<Entity> lane3_hits;
	std::vector<Entity> lane4_hits;
	auto lane_hits = {&lane1_hits, &lane2_hits, &lane3_hits, &lane4_hits};

	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;
		
		// Check for judgment/note collisions specifically
		if (registry.judgmentLine.has(entity) && registry.notes.has(entity_other)) {
			Motion& lane_motion = registry.motions.get(entity);
			float lane = lane_motion.position.x;

			// Collect all successful matches
				// Simple standing feedback using judgement line colour
			if (d_key_pressed && lane == gameInfo.lane_1) {
				lane1_hits.push_back(entity_other);
				handle_note_hit(entity, entity_other);
			}
			else if (f_key_pressed && lane == gameInfo.lane_2) {
				lane2_hits.push_back(entity_other);
				handle_note_hit(entity, entity_other);
			}
			else if (j_key_pressed && lane == gameInfo.lane_3) {
				lane3_hits.push_back(entity_other);
				handle_note_hit(entity, entity_other);
			}
			else if (k_key_pressed && lane == gameInfo.lane_4) {
				lane4_hits.push_back(entity_other);
				handle_note_hit(entity, entity_other);
			}
		}
	}

	// For each lane, remove at most one note (for this frame)
	int got_hit = 0; // 0 if didn't hit any notes, 1 otherwise
	int lane_index = -1;
	for (auto &hits : lane_hits) {

		lane_index += 1;
		// Skip lanes which had no collisions
		if (hits->size() == 0) {
			continue;
		}

		Entity lowest_note = hits->at(0);
		float greatest_y = registry.motions.get(lowest_note).position.y;
		// Find the note with highest y value (furthest down the screen)
		for (int i = 1; i < hits->size(); i++) {
			float note_y = registry.motions.get(hits->at(i)).position.y;

			if (note_y > greatest_y) {
				greatest_y = note_y;
				lowest_note = hits->at(i);
			}
		}

		// Retrieve information about duration before removing notehead from registry
		float hold_duration = -1.0f;
		vec2 position = vec2(0.f);
		if (registry.notes.has(lowest_note)) {
			Note& note = registry.notes.get(lowest_note);
			hold_duration = note.duration;
			Motion& motion = registry.motions.get(lowest_note);
			position = motion.position;
		}

		// Now manage held notes (which does not affect regular note behaviour above)
		if (hold_duration > 0.f) {
			// Spawn a new entity with a duration
			createNoteDuration(renderer, position, hold_duration);
			lane_hold[lane_index] = hold_duration;
		}

		registry.remove_all_components_of(lowest_note);


		got_hit = 1;
	}	

	// Play audio only once per key press
	if (key_pressed) {
		if (got_hit) {
			switch (standing) {
				case perfect:
					audio->playHitPerfect();
					break;
				default:
					audio->playHitGood();
					break;
			}
		} else {
			audio->playMissedNote();
		}
	}

	if (key_pressed && !got_hit) {
		max_combo = max_combo > combo ? max_combo : combo;
		combo = 0;
	}

	// Reset key presses
	key_pressed = false;
	d_key_pressed = false;
	f_key_pressed = false;
	j_key_pressed = false;
	k_key_pressed = false;
};

void Battle::handle_note_hit(Entity entity, Entity entity_other) {
	// Variables to determine score standing
	Motion& lane_motion = registry.motions.get(entity);
	float note_y_pos = registry.motions.get(entity_other).position.y;
	float lane_y_pos = lane_motion.position.y;
	float scoring_margin = (SCORING_LEEWAY / note_travel_time) * gameInfo.height;

	// Sizing
	JudgementLine judgement_line = registry.judgmentLine.get(entity);
	float judgement_line_half_height = lane_motion.scale.y * judgement_line.actual_img_scale_factor;

	// Colour
	vec3& colour = registry.colours.get(entity);
	
	if ((note_y_pos >= lane_y_pos - scoring_margin) && (note_y_pos <= lane_y_pos + scoring_margin)) {
		// set standing to Perfect
		standing = perfect;
		perfect_counter++;
		colour = PERFECT_COLOUR;
		combo++;
	}	// Determine standing
	else if (((note_y_pos >= lane_y_pos - judgement_line_half_height) && (note_y_pos < lane_y_pos - scoring_margin))
		|| ((note_y_pos > lane_y_pos + scoring_margin) && (note_y_pos <= lane_y_pos + judgement_line_half_height))) {
		// set standing to Good
		standing = good;
		good_counter++;
		colour = GOOD_COLOUR;
		combo++;
	}
	else {
		// set standing to Alright
		standing = alright;
		alright_counter++;
		colour = ALRIGHT_COLOUR;
		combo++;
	}

	// Tracking information to recommend timing adjustments
	float displacement = note_y_pos - lane_y_pos;
	if (displacement >= 0.f) {
		count_late += 1;
		total_late_distance += displacement;
	}
	else {
		count_early += 1;
		total_early_distance += displacement;
	}

	// Update score
	score += standing;

	// Render particles
	createSparks(registry.motions.get(entity_other).position);

	// Clean up registry
	// registry.collisionTimers.emplace(entity_other);
	// registry.remove_all_components_of(entity_other); // comment this line out if want node colour change
}

// battle keys:
// DFJK -> rhythm
void Battle::handleRhythmInput(int action, int key) {
	// auto& collisionsRegistry = registry.collisions;
	// auto& collisionsTimerRegistry = registry.collisionTimers;
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_D:
				d_key_pressed = true;
				d_key_held = true;
				break;
			case GLFW_KEY_F:
				f_key_pressed = true;
				f_key_held = true;
				break;
			case GLFW_KEY_J:
				j_key_pressed = true;
				j_key_held = true;
				break;
			case GLFW_KEY_K:
				k_key_pressed = true;
				k_key_held = true;
				break;
			default:
				break;
		}
        // std::cout << "rhythm input: " << key << std::endl;
		if (d_key_pressed || f_key_pressed || j_key_pressed || k_key_pressed) {
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
	// RELEASE (relevant for held notes)
	// If there is a held note, releasing early should be treated as missing a note
	else if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_D:
			d_key_pressed = false;
			d_key_held = false;
			std::cout << "D key released\n";
			if (lane_hold[0] > HOLD_DURATION_LEEWAY) {
				audio->playMissedNote();
				lane_hold[0] = NO_DURATION; // only miss once
			}
			break;
		case GLFW_KEY_F:
			f_key_pressed = false;
			f_key_held = false;
			std::cout << "F key released\n";
			if (lane_hold[1] > HOLD_DURATION_LEEWAY) {
				audio->playMissedNote();
				lane_hold[1] = NO_DURATION;
			}
			break;
		case GLFW_KEY_J:
			j_key_pressed = false;
			j_key_held = false;
			std::cout << "J key released\n";
			if (lane_hold[2] > HOLD_DURATION_LEEWAY) {
				audio->playMissedNote();
				lane_hold[2] = NO_DURATION;
			}
			break;
		case GLFW_KEY_K:
			k_key_pressed = false;
			k_key_held = false;
			std::cout << "K key released\n";
			if (lane_hold[3] > HOLD_DURATION_LEEWAY) {
				audio->playMissedNote();
				lane_hold[3] = NO_DURATION;
			}
			break;
		default:
			break;
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
			case GLFW_KEY_SPACE:
				if (action == GLFW_PRESS) { 
					gameInfo.curr_screen = Screen::OVERWORLD;
					// std::cout << "test return to overworld after battle " << battle_is_over << std::endl;
				}
				break;
			default:
				std::cout << "unhandled key" << std::endl;
				break;
		}
	} else if (!in_countdown) {
		 switch(key) {
			case GLFW_KEY_D:
			case GLFW_KEY_F:
			case GLFW_KEY_J:
			case GLFW_KEY_K:
				handleRhythmInput(action, key);
				break;
			case GLFW_KEY_X:
				handleDebug(action);
				break;
			case GLFW_KEY_MINUS:
				gameInfo.frames_adjustment = max(gameInfo.frames_adjustment - 0.25f, MIN_FRAMES_ADJUSTMENT);
				std::cout << "New timing adjustment: " << gameInfo.frames_adjustment << " frames\n"; 
				break;
			case GLFW_KEY_EQUAL:
				gameInfo.frames_adjustment = min(gameInfo.frames_adjustment + 0.25f, MAX_FRAMES_ADJUSTMENT);
				std::cout << "New timing adjustment: " << gameInfo.frames_adjustment << " frames\n"; 
			default:
				std::cout << "unhandled key" << std::endl;
				break;
		}
	}
};

void Battle::handle_mouse_move(vec2 pos) {
    
};

// Calculate recommended timing adjustment (in ms) for player
	// Only works for small adjustments (note is within collision range)
	// Based on the center of judgment lines
float Battle::calculate_adjustment() {
	std::cout << "EARLY: " << count_early << ", LATE:" << count_late << std::endl;

	int total_count = count_early + count_late;
	float avg_early_distance;
	float avg_late_distance;

	if (count_early > 0) {
		avg_early_distance = total_early_distance / (float)count_early;
	}
	else {
		avg_early_distance = 0.f;
	}


	if (count_late > 0) {
		avg_late_distance = total_late_distance / (float)count_late;
	}
	else {
		avg_late_distance = 0.f;
	}

	if (count_late + count_early == 0) {
		return 0.f;
	}

	float weighted_avg_early_distance = avg_early_distance * count_early / (total_count);
	float weighted_avg_late_distance = avg_late_distance * count_late / (total_count);

	float avg_error_distance = weighted_avg_early_distance + weighted_avg_late_distance;
	float adjustment = avg_error_distance / gameInfo.height * note_travel_time;

	std::cout << "Recommended adjustment: " << adjustment << " ms\n";

	return adjustment;
}

