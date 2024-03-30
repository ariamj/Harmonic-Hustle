#include "battle_screen.hpp"
#include <iostream>
#include <fstream>
#include "../ext/jsoncpp/json/json.h"


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

    loadAllLevelData();
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
	// battleInfo[k].bpm = 130.f;
	// battleInfo[k].metadata_offset = 0.06f * 1000.f;

	bpm_ratio = battleInfo[k].bpm / 60.f;

	battleInfo[k].mode_timings = {
		{0.f, back_and_forth}
	};

	for (int i = 0; i < battleInfo[k].mode_timings.size(); i++) {
		float time = battleInfo[k].mode_timings[i].first;
		float converted_timing = (time * 1000.f / bpm_ratio) - (note_travel_time * timing_offset) - 1.5f * 60.f / bpm_ratio;
		battleInfo[k].mode_timings[i].first = converted_timing;
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

	//for (int i = 0; i < battleInfo[k].count_notes; i++) {
	//	float converted_timing = (1000.f * enemy1_timings[i] / bpm_ratio) + spawn_offset;
	//	battleInfo[k].note_timings.push_back(converted_timing);
	//}

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

	//for (int i = 0; i < battleInfo[k].count_notes; i++) {
	//	float converted_timing = (1000.f * enemy2_timings[i] / bpm_ratio) + spawn_offset;
	//	battleInfo[k].note_timings.push_back(converted_timing);
	//}

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

	//for (int i = 0; i < battleInfo[k].count_notes; i++) {
	//	float converted_timing = (1000.f * enemy3_timings[i] / bpm_ratio) + spawn_offset;
	//	battleInfo[k].note_timings.push_back(converted_timing);
	//}

};

bool Battle::loadAllLevelData() {

    for (int i = 0; i < NUM_UNIQUE_BATTLES; i++) {
        loadLevelFromFile(i);
    }

    return true;
}

bool Battle::loadLevelFromFile(int index) {
    // Create file name string
    std::string file_name;
    file_name = "enemy" + std::to_string(index) + ".json";

    // Attempt to open the file
	std::ifstream file(levels_path(file_name));
    if (!file.is_open()) {
		printf("Enemy data file not found.\n");
        return false;
	}

    // Attempt to parse JSON
	Json::Value root;
	Json::CharReaderBuilder builder;

	try {
		
		builder["collectComments"] = true;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, file, &root, &errs)) {
			Json::StreamWriterBuilder writerBuilder;
			std::string errs_str = Json::writeString(writerBuilder, errs);
			printf("JSON parsing failed: %s\n", errs_str.c_str());
			return false;
		}
		file.close();

        auto beatmaps = root["beatmaps"];

        for (auto beatmap : beatmaps) {
            int difficulty = convertDifficultyToInt(beatmap["difficulty"].asString());
            int battle_index = index + (difficulty * NUM_UNIQUE_BATTLES);

            std::map<std::string, std::vector<NoteInfo>> temp_rhythms;

            // Parse each rhythm into temporary data structure
            for (auto rhythms : beatmap["rhythms"]) {
                // Each rhythm has an identifier name
                for (auto name : rhythms.getMemberNames()) {
                    auto rhythm_data = rhythms[name];
                    // Associate note timings with this rhythm
                    std::vector<NoteInfo> note_infos;
                    for (auto timings : rhythm_data["note_timings"]) {
                        // Read each note's spawn time
                        NoteInfo noteInfo;
                        noteInfo.spawn_time = timings["spawn_time"].asFloat();

                        // Add note info to this rhythm
                        note_infos.push_back(noteInfo);
                    }
                    // Map rhythm name to note infos
                    temp_rhythms[name] = note_infos;
                }
            }

			std::vector<NoteInfo> battle_note_info;

			for (auto rhythm_timing_pair : beatmap["rhythm_timings"]) {
				auto rhythm_names = rhythm_timing_pair.getMemberNames();
				assert(rhythm_names.size() == 1); // There must be exactly be one rhythm id
				auto rhythm_id = rhythm_names[0];

				// Retrieve temp rhythm data constructed previously
				std::vector<NoteInfo> rhythm_note_infos = temp_rhythms[rhythm_id];

				// Get the start time of the rhythm in music
				float start_time = rhythm_timing_pair[rhythm_id].asFloat();

				std::vector<NoteInfo> notes_to_add;
				// Compute spawn times, using start time as the reference point
				for (NoteInfo rhythm_note : rhythm_note_infos) {
					NoteInfo converted_note_info;
					converted_note_info.spawn_time = start_time + rhythm_note.spawn_time;
					notes_to_add.push_back(converted_note_info);
				}

				// Append new notes temporary data structure holding note info
				battle_note_info.insert(battle_note_info.end(), notes_to_add.begin(), notes_to_add.end());
			}

			std::vector<std::pair<float, BattleMode>> mode_timings;

			for (auto mode_timing_pair : beatmap["mode_timings"]) {
				auto mode_names = mode_timing_pair.getMemberNames();
				assert(mode_names.size() >= 1); // There must be at least one mode

				for (auto mode : mode_names) {
					std::pair<float, BattleMode> mode_timing;
					mode_timing.first = mode_timing_pair[mode].asFloat();
					mode_timing.second = convertStringToBattleMode(mode);
					mode_timings.push_back(mode_timing);
				}
			}

			// Set game-persistent battle info using parsed data
			// Offset and BPM
			battleInfo[battle_index].metadata_offset = root["metadata_offset"].asFloat();
			battleInfo[battle_index].bpm = root["bpm"].asFloat();

			// Must be done after loading bpm
			convertBeatsToMilliseconds(&battle_note_info, battleInfo[battle_index].bpm / 60.f);

			// Note timings
			battleInfo[battle_index].note_timings = battle_note_info;
			battleInfo[battle_index].count_notes = battleInfo[battle_index].note_timings.size();

			// Mode timings
			battleInfo[battle_index].mode_timings = mode_timings;
        }

	}
	catch (std::exception e) {
		printf("Exception reading!\n");
		return false;
	}

	return true;
}

int Battle::convertDifficultyToInt(std::string difficulty) {
    if (difficulty == "normal") {
        return 0;
    } else if (difficulty == "hard") {
        return 1;
    } else {
        printf("Invalid difficulty in JSON\n");
        return -1;
    }
}

// Convert 0-indexed metronome-based beats to timings in milliseconds
void Battle::convertBeatsToMilliseconds(std::vector<NoteInfo> *note_infos, float bpm_ratio) {
	for (int i = 0; i < note_infos->size(); i++) {
		float converted_timing = (1000.f * note_infos->at(i).spawn_time / bpm_ratio) + spawn_offset;
		note_infos->at(i).spawn_time = converted_timing;
	}
}

BattleMode Battle::convertStringToBattleMode(std::string mode_string) {
	if (mode_string == "bnf") {
		return back_and_forth;
	}
	else if (mode_string == "beat_rush") {
		return beat_rush;
	}
	else if (mode_string == "unison") {
		return unison;
	}
	else {
		printf("Invalid mode in JSON; returning back_and_forth as default");
		return back_and_forth;
	}
}