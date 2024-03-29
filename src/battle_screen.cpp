#include "battle_screen.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include <audio_system.hpp>
#include "chrono"

using Clock = std::chrono::high_resolution_clock;

// consts for now;
const size_t MAX_NOTES = 10;
// const size_t NOTE_SPAWN_DELAY = 2000;
const vec3 PERFECT_COLOUR = { 255.f, 1.f, 255.f };
const vec3 GOOD_COLOUR = { 1.f, 255.f, 1.f };
const vec3 ALRIGHT_COLOUR = { 255.f, 255.f, 1.f };
const vec3 MISSED_COLOUR = { 255.f, 1.f, 1.f };

const float APPROX_FRAME_DURATION = 16.6f;
const float SCORING_LEEWAY = 1.2f * APPROX_FRAME_DURATION; // higher is easier to score better

// the time it should take for note to fall from top to bottom
// TODO: Allow calibration via difficulty setting
float note_travel_time = 2000.f;

// rhythmic input timing variables, initialized in .init
float spawn_offset; 
// TODO: Allow calibration by player.
float adjust_offset = 0.00f; // default from testing manually.
float adjust_increment = 0.002f; // very small changes are impactful
float timing_offset = 1 - (1.f / (1.2f + adjust_offset)); // coupled with judgment_y_pos in createJudgmentLine
float top_to_judgment = note_travel_time * (1 - timing_offset); // time it takes from top edge to judgment lines

// battle mode tracker
int mode_index;
float next_mode_delay;

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


Serializer s = Serializer();

Battle::Battle() {
    rng = std::default_random_engine(std::random_device()());
}

Battle::~Battle() {

};

void Battle::init(GLFWwindow* window, RenderSystem* renderer, AudioSystem* audio, Serializer* saver) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
	this->audio = audio;
	this->saver = saver;

	lanes[0] = gameInfo.lane_1;
    lanes[1] = gameInfo.lane_2;
    lanes[2] = gameInfo.lane_3;
    lanes[3] = gameInfo.lane_4;

	// Used to spawn notes relative to judgment line instead of window height
		// Divide by 1000.f if using song position
	spawn_offset = -(note_travel_time - (note_travel_time * (timing_offset)));

	float bpm_ratio;

	// Load battle-specific data into BattleInfo structs
	// OPTIMIZE: Read these from a file instead
	std::vector<float> enemy0_timings = { 
		// BACK AND FORTH
		4.f, 5.f, 6.f, 6.5f, 7.f,
		12.f, 13.f, 14.f, 14.5f, 15.f,
		20.f, 21.f, 22.f, 22.5f, 23.f,
		28.f, 29.f, 30.f, 30.5f, 31.f,
		40.f, 41.f, 42.f, 43.f, 44.f, 45.5f,
		56.f, 57.f, 58.f, 59.f, 60.f, 61.5f 
	};
	int k = 0;
	battleInfo[k].count_notes = enemy0_timings.size();
	battleInfo[k].bpm = 130.f;
	battleInfo[k].metadata_offset = 0.06f * 1000.f;

	bpm_ratio = battleInfo[k].bpm / 60.f;

	battleInfo[k].mode_timings = {
		{0.f, back_and_forth}
	};

	for (int i = 0; i < battleInfo[k].mode_timings.size(); i++) {
		float time = battleInfo[k].mode_timings[i].first;
		float converted_timing = (time * 1000.f / bpm_ratio) - (note_travel_time * timing_offset) - 1.5f * 60.f / bpm_ratio;
		battleInfo[k].mode_timings[i].first = converted_timing;
	}

	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy0_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

	// Another battle
	std::vector<float> enemy1_timings = { 
		// BACK AND FORTH
		8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 13.5f, 14.5f, 15.f,
		24.f, 25.f, 26.f, 27.f, 28.f, 29.f, 29.5f, 30.5f, 31.f,
		40.f, 41.f, 42.f, 43.f, 44.f, 45.f, 45.5f, 46.5f, 47.f,
		56.f, 57.f, 58.f, 59.f, 60.f, 61.f, 61.5f, 62.5f, 63.f,
		// BEAT RUSH
		64.f, 65.f, 66.f, 67.f, 68.f, 69.f, 70.f, 71.f,
		72.f, 73.f, 74.f, 75.f, 76.f, 77.f, 78.f, 79.f,
		80.f, 81.f, 82.f, 83.f, 84.f, 85.f, 86.f, 87.f,
		88.f, 89.f, 90.f, 91.f, 92.f, 93.f, 94.f, 95.f, 96.f, 97.f, 98.f, 99.f,
		// BACK AND FORTH
		116.f, 118.f, 120.f, 122.f, 123.f, 130.f, 131.f,
		148.f, 150.f, 152.f, 154.f, 156.f, 158.f, 159.f 
	};
	k = 1;
	battleInfo[k].count_notes = enemy1_timings.size();
	battleInfo[k].bpm = 184.f;
	battleInfo[k].metadata_offset = 0.0675f * 1000.f;

	bpm_ratio = battleInfo[k].bpm / 60.f;

	battleInfo[k].mode_timings = {
		{0.f, back_and_forth}, 
		{64.f, beat_rush},
		{112.f - 64.f, back_and_forth}}; // earlier due to pause in music

	for (int i = 1; i < battleInfo[k].mode_timings.size(); i++) {
		float time = battleInfo[k].mode_timings[i].first;
		float converted_timing = (time * 1000.f / bpm_ratio) - (note_travel_time * timing_offset) - 1.5f * 60.f / bpm_ratio;
		battleInfo[k].mode_timings[i].first = converted_timing;
	}

	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy1_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

	// Another battle
	std::vector<float> enemy2_timings = { 
		// BACK AND FORTH
		8.f, 8.5f, 9.f, 10.f, 11.f, 12.f, 12.5f, 13.f, 13.5f,
		24.f, 24.5f, 25.f, 26.f, 27.f, 28.f, 28.5f, 29.f, 29.5f,

		40.f, 41.f, 42.f, 43.f, 44.f, 45.f, 45.5f, 46.5f, 47.f,
		56.f, 57.f, 58.f, 59.f, 60.f, 61.f, 61.5f, 62.5f, 63.f,

		// UNISON
		64.f, 64.5f, 65.f, 66.f, 67.f, 68.f, 68.5f, 69.f, 69.5f,
		72.f, 72.5f, 73.f, 74.f, 75.f, 76.f, 76.5f, 77.f, 77.5f,
		80.f, 80.5f, 81.f, 82.f, 83.f, 84.f, 84.5f, 85.f, 85.5f, 
		88.f, 88.5f, 89.f, 90.f, 91.f, 92.f, 92.5f, 93.f, 93.5f,
		94.5f, 95.f, 95.5f,

		// BACK AND FORTH
		104.f, 105.5f, 106.f, 107.5, 108.f, 109.f, 110.f, 110.5f, 111.f,
		120.f, 121.5f, 122.f, 123.5f, 124.f, 125.f, 126.f, 126.5f, 127.f,

		136.f, 137.f, 138.f, 139.f, 140.f, 141.f, 141.5f, 142.5f, 143.f,
		152.f, 153.f, 154.f, 155.f, 156.f, 157.f, 157.5f, 158.5f, 159.f,

		// UNISON
		160.f, 161.f, 162.f, 163.f, 164.f, 165.f, 165.5f, 166.5f, 167.f,
		168.f, 168.5f, 169.f, 170.f, 171.f, 172.f, 172.5f, 173.f, 173.5f,
		176.f, 177.f, 178.f, 179.f, 180.f, 181.f, 181.5f, 182.5f, 183.f,
		184.f, 184.5f, 185.f, 186.f, 187.f, 188.f, 188.5f, 189.f, 189.5f, 
		190.5, 191.f, 191.5f
	};
	k = 2;
	battleInfo[k].count_notes = enemy2_timings.size();
	battleInfo[k].bpm = 152.f;
	battleInfo[k].metadata_offset = 0.05f * 1000.f;

	bpm_ratio = battleInfo[k].bpm / 60.f;

	battleInfo[k].mode_timings = {
		{0.f, back_and_forth}, 
		{64.f, unison},
		{100.f - 64.f, back_and_forth},
		{160.f - 100.f, unison}};

	for (int i = 0; i < battleInfo[k].mode_timings.size(); i++) {
		float time = battleInfo[k].mode_timings[i].first;
		float converted_timing = (time * 1000.f / bpm_ratio) - (note_travel_time * timing_offset) - 1.5f * 60.f / bpm_ratio;
		battleInfo[k].mode_timings[i].first = converted_timing;
	}

	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy2_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

	// Boss battle
	// CODING AT ITS FINEST..... TRULY
	// OPTIMIZE: Create "Rhythm" presets to reduce this workload to every line, rather than every note
	std::vector<float> enemy3_timings = { 
		// BACK AND FORTH
		4.f, 4.5f, 5.f, 5.5f, 6.f, 6.5f, 6.75f, 7.25f, 7.5f,
		12.f, 12.5f, 13.f, 13.5f, 14.f, 14.5f, 14.75f, 15.25f, 15.5f,
		20.f, 20.5f, 21.f, 21.5f, 22.f, 22.5f, 22.75f, 23.25f, 23.5f,
		28.f, 28.5f, 29.f, 29.5f, 30.f, 30.25f, 30.5f, 30.75f, 31.25f, 31.5f,

		// UNISON
		32.f, 32.5f, 33.f, 33.5f, 34.f, 34.5f, 34.75f,
		36.f, 36.5f, 37.f, 37.5f, 38.f, 38.5f, 38.75f,
		40.f, 40.5f, 41.f, 41.5f, 42.f, 42.5f, 42.75f,
		44.f, 44.5f, 45.f, 45.5f, 46.f, 46.5f, 46.75f, 

		// BEAT RUSH
		48.f, 48.5f, 49.f, 49.5f, 50.f, 50.5f, 51.f, 51.5f, 52.f, 52.5f, 53.f, 53.5f, 54.f, 54.5f, 55.f, 55.5f,
		56.f, 56.5f, 57.f, 57.5f, 58.f, 58.5f, 59.f, 59.5f, 60.f, 60.5f, 61.f, 61.5f, 62.f, 62.5f, 63.f, 63.5f,
		64.f, 64.5f, 65.f, 65.5f, 66.f, 66.5f, 67.f, 67.5f, 68.f, 68.5f, 69.f, 69.5f, 70.f, 70.5f, 71.f, 71.5f,
		72.f, 72.5f, 73.f, 73.5f, 74.f, 74.5f, 75.f, 75.5f, 76.f, 76.5f, 77.f, 77.5f, 78.f, 78.5f, 79.f, 79.5f,

		// UNISON
		80.f, 80.5f, 81.f, 81.5f, 82.f, 82.5f, 82.75f, 83.25f, 83.5f,
		85.5f, 86.f, 87.5f,
		88.f, 88.5f, 89.f, 89.5f, 90.f, 90.5f, 90.75f, 91.25f, 91.5f,
		93.5f, 94.f, 94.5, 95.f, 95.5f,
		96.f, 96.5f, 97.f, 97.5f, 98.f, 98.5f, 98.75f, 99.25f, 99.5f,
		101.5f, 102.f, 103.5f,
		104.f, 104.5, 105.f, 105.5, 106.f, 106.5, 106.75f, 107.25f, 107.5f,

		// BEAT RUSH
		128.f, 128.5f, 129.f, 129.5f, 130.f, 130.5, 131.f, 131.5f, 132.f, 132.5f, 133.f, 133.5f, 134.f, 134.5f, 135.f, 135.5f,
		136.f, 136.5f, 137.f, 137.5f, 138.f, 138.5, 139.f, 139.5f, 140.f, 140.5f, 141.f, 141.5f, 142.f, 142.5f, 143.f, 143.5f,
		144.f, 144.5f, 145.f, 145.5f, 146.f, 146.5f, 147.f, 147.5f, 148.f, 148.5f, 149.f, 149.5f, 150.f, 150.5f, 151.f, 151.5f,
		152.f, 152.5f, 153.f, 153.5f, 154.f, 154.5f, 155.f, 155.5f, 156.f, 156.5f, 157.f, 157.5f, 158.f, 158.5f, 159.f, 159.5f,

		// UNISON
		160.f, 160.5f, 161.f, 161.5f, 162.f, 162.5f, 162.75f, 163.25f, 163.5f,
		165.5f, 166.f, 166.5f, 167.f, 167.5f,
		168.f, 168.5f, 169.f, 169.5f, 170.f, 170.5f, 170.75f, 171.25f, 171.5f,
		172.5f, 173.f, 173.5f, 174.f, 174.5f, 175.f, 175.5f,
		176.f, 176.5f, 177.f, 177.5f, 178.f, 178.5f, 178.75f, 179.25f, 179.5f,
		181.5f, 182.f, 182.5f, 183.f, 183.5f,
		184.f, 184.5f, 185.f, 185.5f, 186.f, 186.5f, 186.75f, 187.25f, 187.5f,
		188.f, 188.5f, 189.f, 189.25f, 189.5f, 190.f, 190.5f, 191.f, 191.25f, 191.5f,

		192.f, 193.f, 194.f, 194.5f, 195.f,
		196.f, 196.5f, 197.f, 197.5f, 198.f, 198.5, 199.f, 199.5f,
		200.f, 201.f, 202.f, 202.5f, 203.f,
		204.f, 204.5f, 205.f, 205.5f, 206.f, 206.5, 207.f, 207.5f,
		208.f, 209.f, 210.f, 210.5f, 211.f,
		212.f, 212.5f, 213.f, 213.5f, 214.f, 214.5, 215.f, 215.5f,
		216.f, 217.f, 218.f, 218.5f, 219.f,
		220.f, 220.5f, 221.f, 221.5f, 222.f, 222.5, 223.f, 223.5f, 224.f
	};
	k = 3;
	battleInfo[k].count_notes = enemy3_timings.size();
	battleInfo[k].bpm = 128.f;
	battleInfo[k].metadata_offset = 0.07f * 1000.f;

	bpm_ratio = battleInfo[k].bpm / 60.f;

	battleInfo[k].mode_timings = {
		{0.f, back_and_forth}, 
		{32.f, unison},
		{48.f - 32.f, beat_rush},
		{80.f - 48.f, unison},
		{124.f - 80.f, beat_rush}, // earlier due to pause in music
		{160.f - 124.f, unison}
	};

	for (int i = 0; i < battleInfo[k].mode_timings.size(); i++) {
		float time = battleInfo[k].mode_timings[i].first;
		float converted_timing = (time * 1000.f / bpm_ratio) - (note_travel_time * timing_offset) - 1.5f * 60.f / bpm_ratio;
		battleInfo[k].mode_timings[i].first = converted_timing;
	}

	for (int i = 0; i < battleInfo[k].count_notes; i++) {
		float converted_timing = (1000.f * enemy3_timings[i] / bpm_ratio) + spawn_offset;
		battleInfo[k].note_timings.push_back(converted_timing);
	}

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

		// notify save
		createText("Game saved", vec2(gameInfo.width / 2.f, gameInfo.height / 2.f - (spacing * 6)), 0.4f, Colour::green, glm::mat4(1.f), Screen::BATTLE, true);
	} else {
		auto& motions_registry = registry.motions;

		// Process the player state
		// assert(registry.screenStates.components.size() <= 1);
		// ScreenState& screen = registry.screenStates.components[0];

		float min_counter_ms = 3000.f;

		// Update song position 
		float new_song_position = audio->getSongPosition() * 1000.f - conductor.offset;
		std::cout << "Considering new song position:" << new_song_position;

		std::cout << "... Conductor song position changed from:" << conductor.song_position;
		if (new_song_position > conductor.song_position) {
			conductor.song_position = new_song_position;
		}
		// Check for negative (due to offset) to prevent elapsed_ms from taking over
		else if (new_song_position > 0.f) {
			// Use elapsed-time when consecutive queries return same value
			conductor.song_position += elapsed_ms_since_last_update;
		}

		std::cout << " to:" << conductor.song_position << "\n";

		next_note_spawn -= elapsed_ms_since_last_update;

		// Update battle mode
		next_mode_delay -= elapsed_ms_since_last_update;
		if (next_mode_delay <= 0.f && mode_index < battleInfo[enemy_index].mode_timings.size()) {
			switch (battleInfo[enemy_index].mode_timings[mode_index].second) {
				case back_and_forth:
					gameInfo.battleModeColor = {-1.f, 0.2f, 1.f, 0.f}; // no adjust
					break;
				case beat_rush:
					gameInfo.battleModeColor = {1.5f, -0.2f, -0.2f, -0.2f}; // adjust to red
					break;
				case unison:
					gameInfo.battleModeColor = {1.f, -0.2f, -1.f, 0.f}; // adjust to orange
					break;						
			}
			mode_index += 1;
			if (mode_index < battleInfo[enemy_index].mode_timings.size()) {
				next_mode_delay += battleInfo[enemy_index].mode_timings[mode_index].first;
			}
		}

		// TODO (?): Initiate some visual FX on every beat of song
		// Track each beat of song
		if (conductor.song_position > last_beat + conductor.crotchet) {
			 // std::cout << "Beat detected" << "\n";
			 last_beat += conductor.crotchet;
		}

		// Spawning notes based on song position
		if (next_note_index < num_notes) {
			float note_spawn_time = battleInfo[enemy_index].note_timings[next_note_index];
			if (conductor.song_position >= note_spawn_time) {
				createNote(renderer, vec2(lanes[rand() % 4], 0.f), note_spawn_time);
				next_note_index += 1;
			}
		}

		//
		//// Spawning notes based on elapsed time
		//if (registry.notes.components.size() < MAX_NOTES && next_note_spawn < 0.f && next_note_index <= num_notes) {
		//	// spawn notes in the four lanes
		//	createNote(renderer, vec2(lanes[rand() % 4], 0.f));

		//	if (next_note_index < num_notes) {
		//		// set next timer, subtracting the "overshot" time (next_note_spawn <= 0.f) during this frame
		//		next_note_spawn = battleInfo[enemy_index].note_timings[next_note_index]
		//						- battleInfo[enemy_index].note_timings[next_note_index - 1]
		//						+ next_note_spawn;
		//	}

		//	next_note_index += 1;
		//}

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

		//// update notes positions
		//for (int i = 0; i < registry.motions.components.size(); ++i) {
		//	Motion& motion = registry.motions.components[i];

		//	if (registry.notes.has(motions_registry.entities[i])) {
		//		// Increment progress on range [0,1]
		//		float progress_step = elapsed_ms_since_last_update / note_travel_time;
		//		motion.progress = min(1.f, motion.progress + progress_step);

		//		// Interpolate note position from top to bottom of screen
		//		motion.position.y = lerp(0.0, float(gameInfo.height), motion.progress);

		//		// Interpolate note size, increasing from top (1x) to bottom (2.5x) of screen
		//		motion.scale_factor = lerp(1.0, NOTE_MAX_SCALE_FACTOR, motion.progress);
		//	}
		//}

		// Update note positions based on conductor time
		for (auto entity : registry.notes.entities) {
			if (!registry.motions.has(entity)) {
				continue;
			}
			Note& note = registry.notes.get(entity);
			Motion& motion = registry.motions.get(entity);

			motion.position.y = lerp(0.0, float(gameInfo.height), (conductor.song_position - note.spawn_time) / note_travel_time);
			motion.scale_factor = lerp(1.0, NOTE_MAX_SCALE_FACTOR, (conductor.song_position - note.spawn_time) / note_travel_time);	
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
	// Delete any remaining sparks entities
	for (auto entity : registry.particleEffects.entities) {
		registry.remove_all_components_of(entity);
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

	mode_index = 0;
	next_mode_delay = 0.f;
	
	// Set Conductor variables
	conductor.bpm = battleInfo[enemy_index].bpm;
	conductor.crotchet = 60.f / battleInfo[enemy_index].bpm;
	conductor.offset = battleInfo[enemy_index].metadata_offset;
	conductor.song_position = 0.f; //what should this be?
	last_beat = 0.f; // moving reference point

	// Reset score
	score = 0;
	// Reset score threshold
	score_threshold = ceil(num_notes * perfect / 2);
	// score_threshold = 0;

	// Reset counters
	perfect_counter = 0;
    good_counter = 0;
    alright_counter = 0;
    missed_counter = 0;

	// TODO: Account for when note spawns are negative (before music starts)
	next_note_spawn = battleInfo[enemy_index].note_timings[0];
	next_note_index = 0; // 0 for song_position based, 1 for elapsed_time based

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
	for (auto &hits : lane_hits) {
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
	}	// Determine standing
	else if (((note_y_pos >= lane_y_pos - judgement_line_half_height) && (note_y_pos < lane_y_pos - scoring_margin))
		|| ((note_y_pos > lane_y_pos + scoring_margin) && (note_y_pos <= lane_y_pos + judgement_line_half_height))) {
		// set standing to Good
		standing = good;
		good_counter++;
		colour = GOOD_COLOUR;
	}
	else {
		// set standing to Alright
		standing = alright;
		alright_counter++;
		colour = ALRIGHT_COLOUR;
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
					// std::cout << "test return to overworld after battle " << battle_is_over << std::endl;
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
