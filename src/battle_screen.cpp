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
// const size_t NOTE_SPAWN_DELAY = 2000;
const vec3 PERFECT_COLOUR = { 255.f, 1.f, 255.f };
const vec3 GOOD_COLOUR = { 1.f, 255.f, 1.f };
const vec3 ALRIGHT_COLOUR = { 255.f, 255.f, 1.f };
const vec3 MISSED_COLOUR = { 255.f, 1.f, 1.f };

// the time it should take for note to fall from top to bottom
const float NOTE_TRAVEL_TIME = 2000.f;

// rhythmic input timing variables, initialized in .init
float spawn_offset; 

// battle-specific variables for readability, initialized in .start
int enemy_index;
int num_notes;
int next_note_index;
float last_beat;

// Enemy-specific battle information
const int NUM_UNIQUE_BATTLES = 4;
BattleInfo battleInfo[NUM_UNIQUE_BATTLES];


// DEBUG MEMORY LEAKS (WINDOWS ONLY)
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/find-memory-leaks-using-the-crt-library?view=msvc-170
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//_CrtMemState s1;
//_CrtMemState s2;


Battle::Battle() {
    rng = std::default_random_engine(std::random_device()());
}

Battle::~Battle() {

};

void Battle::init(GLFWwindow* window, RenderSystem* renderer, AudioSystem* audio) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
	this->audio = audio;

	lanes[0] = gameInfo.lane_1;
    lanes[1] = gameInfo.lane_2;
    lanes[2] = gameInfo.lane_3;
    lanes[3] = gameInfo.lane_4;

	// Used to spawn notes relative to judgment line instead of window height
		// Divide by 1000.f if using song position
	spawn_offset = -(NOTE_TRAVEL_TIME - (NOTE_TRAVEL_TIME * (1 - 1.f / 1.225f)));

	float bpm_ratio;

	// Load battle-specific data into BattleInfo structs
	// OPTIMIZE: Read these from a file instead
	int k = 0;
	battleInfo[k].count_notes = 32;
	battleInfo[k].bpm = 130.f;

	std::vector<float> enemy0_timings = { 4.f, 5.f, 6.f, 6.5f, 7.f,
										12.f, 13.f, 14.f, 14.5f, 15.f,
										20.f, 21.f, 22.f, 22.5f, 23.f,
										28.f, 29.f, 30.f, 30.5f, 31.f,
										40.f, 41.f, 42.f, 43.f, 44.f, 45.5f,
										56.f, 57.f, 58.f, 59.f, 60.f, 61.5f };
	bpm_ratio = battleInfo[k].bpm / 60.f;
	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy0_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

	// Another battle
	k = 1;
	battleInfo[k].count_notes = 70;
	battleInfo[k].bpm = 184.f;

	std::vector<float> enemy1_timings = { 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 13.5f, 14.5f, 15.f,
										24.f, 25.f, 26.f, 27.f, 28.f, 29.f, 29.5f, 30.5f, 31.f,
										40.f, 41.f, 42.f, 43.f, 44.f, 45.f, 45.5f, 46.5f, 47.f,
										56.f, 57.f, 58.f, 59.f, 60.f, 61.f, 61.5f, 62.5f, 63.f,
										80.f, 81.f, 82.f, 83.f, 84.f, 85.f, 86.f, 87.f,
										88.f, 89.f, 90.f, 91.f, 92.f, 93.f, 94.f, 95.f, 96.f, 97.f, 98.f, 99.f,
										116.f, 118.f, 120.f, 122.f, 123.f, 130.f, 131.f,
										148.f, 150.f, 152.f, 154.f, 156.f, 158.f, 159.f };
	bpm_ratio = battleInfo[k].bpm / 60.f;
	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy1_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

	// Another battle
	k = 2;
	battleInfo[k].count_notes = 36;
	battleInfo[k].bpm = 152.f;


	std::vector<float> enemy2_timings = { 8.f, 8.5f, 9.f, 10.f, 11.f, 12.f, 12.5f, 13.f, 13.5f,
										24.f, 24.5f, 25.f, 26.f, 27.f, 28.f, 28.5f, 29.f, 29.5f,
										40.f, 41.f, 42.f, 43.f, 44.f, 45.f, 45.5f, 46.5f, 47.f,
										56.f, 57.f, 58.f, 59.f, 60.f, 61.f, 61.5f, 62.5f, 63.f };
	bpm_ratio = battleInfo[k].bpm / 60.f;
	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy2_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

	//TODO: JUST PLACEHOLDER DATA FOR NOW
	k = 3;
	battleInfo[k].count_notes = 36;
	battleInfo[k].bpm = 152.f;


	std::vector<float> enemy3_timings = { 8.f, 8.5f, 9.f, 10.f, 11.f, 12.f, 12.5f, 13.f, 13.5f,
										24.f, 24.5f, 25.f, 26.f, 27.f, 28.f, 28.5f, 29.f, 29.5f,
										40.f, 41.f, 42.f, 43.f, 44.f, 45.f, 45.5f, 46.5f, 47.f,
										56.f, 57.f, 58.f, 59.f, 60.f, 61.f, 61.5f, 62.5f, 63.f };
	bpm_ratio = battleInfo[k].bpm / 60.f;
	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy3_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

};

bool Battle::handle_step(float elapsed_ms_since_last_update, float current_speed) {
	std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Battle";
	title_ss << " --- FPS: " << FPS;
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

		// next instruction
		createText("Press space to continue", vec2(gameInfo.width/2.f, gameInfo.height/2.f + (spacing * 4)), 0.4f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
	} else {
		auto& motions_registry = registry.motions;

		// Process the player state
		// assert(registry.screenStates.components.size() <= 1);
		// ScreenState& screen = registry.screenStates.components[0];

		float min_counter_ms = 3000.f;
		next_note_spawn -= elapsed_ms_since_last_update;

		// Update song position every frame
		// SADNESS: Only changes in value every 5-8 frames :( 
			// - Notes will stutter if used for interpolation every frame
			// - Slightly less accurate spawning compared to using elapsed time
			// + Safeguards against any delay in starting the music
		// Could still be useful for visual FX that happen periodically
		conductor.song_position = audio->getSongPosition();
		// std::cout << conductor.song_position << "\n";

		// Track each beat of song
		if (conductor.song_position > last_beat + conductor.crotchet) {
			// TODO (?): Initiate some visual FX on every beat of song
			std::cout << "Beat detected" << "\n";
			last_beat += conductor.crotchet;
		}

		// Spawning notes based on song position
		/*
		if (next_note_index < num_notes) {
			if (conductor.song_position > battleInfo[enemy_index].note_timings[next_note_index]) {
				createNote(renderer, vec2(lanes[rand() % 4], 0.f));
				next_note_index += 1;
			}
		}
		*/
		
		// Spawning notes based on elapsed time
		if (registry.notes.components.size() < MAX_NOTES && next_note_spawn < 0.f && next_note_index <= num_notes) {
			// spawn notes in the four lanes
			createNote(renderer, vec2(lanes[rand() % 4], 0.f));

			if (next_note_index < num_notes) {
				// set next timer, subtracting the "overshot" time (next_note_spawn <= 0.f) during this frame
				next_note_spawn = battleInfo[enemy_index].note_timings[next_note_index]
								- battleInfo[enemy_index].note_timings[next_note_index - 1]
								+ next_note_spawn;
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
				if (registry.notes.has(motions_registry.entities[i])) {
					audio->playDroppedNote();
					standing = missed;
					missed_counter++;
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
		renderer->updateParticles(elapsed_ms_since_last_update);
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

	std::cout << "Battle over popup: press SPACE to continue" << std::endl;
	// Only process events once
	if (battle_is_over) {
		return;
	}
	setBattleIsOver(true);

	// Delete any remaining note entities
	for (auto entity : registry.notes.entities) {
		registry.remove_all_components_of(entity);
	}

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

		if (gameInfo.curr_level != gameInfo.max_level) {
			registry.levels.get(*gameInfo.player_sprite).level++;
		}
	// battle lost
	} else {
		// remove colllided with enemy (give player another chance)
		registry.enemies.remove(gameInfo.curr_enemy);
		registry.renderRequests.remove(gameInfo.curr_enemy);

	}
	gameInfo.curr_enemy = {};
	renderer->updateParticles(0.f);
	renderer->particle_generators.clear();


	// DEBUG MEMORY LEAKS (WINDOWS ONLY)
	//_CrtMemCheckpoint(&s2);
	//_CrtMemState s3;
	//if (_CrtMemDifference(&s3, &s1, &s2)) {

	//	_CrtMemDumpStatistics(&s3);
	//}
	
}

void Battle::start() {
	// DEBUG MEMORY LEAKS (WINDOWS ONLY)
	 //_CrtMemCheckpoint(&s1);

	// Local variables to improve readability
	enemy_index = min(gameInfo.curr_level - 1, NUM_UNIQUE_BATTLES - 1); // -1 for 0-indexing
	num_notes = battleInfo[enemy_index].count_notes;

	// Set Conductor variables
	conductor.bpm = battleInfo[enemy_index].bpm;
	conductor.crotchet = 60.f / battleInfo[enemy_index].bpm;
	conductor.offset = 0.f; // unused right now.
	last_beat = 0.f; // moving reference point

	// Reset score
	score = 0;
	// Reset score threshold
	enemy_battle_sprite = gameInfo.curr_enemy;
	if (registry.battleProfiles.has(enemy_battle_sprite)) {
		score_threshold = registry.battleProfiles.get(enemy_battle_sprite).score_threshold;
	}
	// Reset counters
	perfect_counter = 0;
    good_counter = 0;
    alright_counter = 0;
    missed_counter = 0;

	std::cout << "Starting battle against enemy index: " << enemy_index << "\n";

	// TODO: Account for when note spawns are negative (before music starts)
	next_note_spawn = battleInfo[enemy_index].note_timings[0];
	next_note_index = 1; // Set to 0 if using song position

	if (gameInfo.curr_level == 4) {
		Entity e = registry.battleEnemy.entities[0];

		RenderRequest& render = registry.renderRequests.get(e);
		render.used_texture = TEXTURE_ASSET_ID::BATTLEBOSS;
	}

	// Create generators for particles that appear in the battle scene
	renderer->createParticleGenerator((int)PARTICLE_TYPE_ID::TRAIL);

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
						alright_counter++;
						colour = ALRIGHT_COLOUR;
					} else if (((note_y_pos >= lane_y_pos - judgement_line_half_height) && (note_y_pos < lane_y_pos - scoring_margin))
								|| ((note_y_pos > lane_y_pos + scoring_margin) && (note_y_pos <= lane_y_pos + judgement_line_half_height))) {
						// set standing to Good
						standing = good;
						good_counter++;
						colour = GOOD_COLOUR;
					} else if ((note_y_pos >= lane_y_pos - scoring_margin) && (note_y_pos <= lane_y_pos + scoring_margin)) {
						// set standing to Perfect
						standing = perfect;
						perfect_counter++;
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
			audio->playHitPerfect();
		}
		else {
			audio->playMissedNote(); // placeholder sound effect
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
void handleRhythmInput(int action, int key) {
	// auto& collisionsRegistry = registry.collisions;
	// auto& collisionsTimerRegistry = registry.collisionTimers;
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
			default:
				std::cout << "unhandled key" << std::endl;
				break;
		}
	}
};

void Battle::handle_mouse_move(vec2 pos) {
    
};
