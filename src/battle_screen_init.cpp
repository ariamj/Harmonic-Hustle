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
	spawn_offset = -(note_travel_time - (note_travel_time * (timing_offset)));

	// Load level data from file :)
    loadAllLevelData();
};

bool Battle::loadAllLevelData() {

    for (int i = 0; i < NUM_UNIQUE_BATTLES; i++) {
        loadLevelFromFile(i);
    }

    return true;
}

// Load data for a single level from a JSON file
// Adapted from serializer.cpp
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
			int count_held_notes = 0;

			// Offset and BPM
			battleInfo[battle_index].metadata_offset = root["metadata_offset"].asFloat();
			battleInfo[battle_index].bpm = root["bpm"].asFloat();

            std::map<std::string, std::vector<NoteInfo>> temp_rhythms;

            // Parse each rhythm into temporary data structure
            for (auto rhythms : beatmap["rhythms"]) {
                // Each rhythm has an identifier name
                for (auto name : rhythms.getMemberNames()) {
                    auto rhythm_data = rhythms[name];
                    // Associate note timings with this rhythm
                    std::vector<NoteInfo> note_infos;
                    for (auto timings : rhythm_data["note_timings"]) {
                        // Read each note's data within a defined rhythm
                        NoteInfo noteInfo;
                        noteInfo.spawn_time = timings["spawn_time"].asFloat();
						noteInfo.quantity = timings["quantity"].asInt();
						noteInfo.duration = timings["duration"].asFloat();

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
					// Set each note's data based on defined rhythm
					NoteInfo converted_note_info;
					converted_note_info.spawn_time = start_time + rhythm_note.spawn_time;
					converted_note_info.quantity = 1; // push back multiple copies instead
					converted_note_info.duration = rhythm_note.duration * 60.f / battleInfo[battle_index].bpm * 1000.f;

					if (rhythm_note.duration > 0.f) {
						count_held_notes += 1;
					}

					// Add duplicate copies for multiple notes
					for (int i = 0; i < rhythm_note.quantity; i++) {
						notes_to_add.push_back(converted_note_info);
					}
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

			// Must be done after loading bpm
			convertBeatsToMilliseconds(&battle_note_info, battleInfo[battle_index].bpm / 60.f);
			convertBeatsToMilliseconds(&mode_timings, battleInfo[battle_index].bpm / 60.f);

			// Note timings
			// Sort notes by spawn times
			std::sort(battle_note_info.begin(), battle_note_info.end(), compareSpawnTimes);
			battleInfo[battle_index].notes = battle_note_info;
			battleInfo[battle_index].count_notes = battleInfo[battle_index].notes.size();
			battleInfo[battle_index].count_held_notes = count_held_notes;

			// Mode timings
  			battleInfo[battle_index].modes = mode_timings;
        }

	}
	catch (std::exception e) {
		printf("Exception reading!\n");
		return false;
	}

	return true;
}

bool Battle::compareSpawnTimes(const NoteInfo& a, const NoteInfo& b)
{
	return a.spawn_time < b.spawn_time;
}

int Battle::convertDifficultyToInt(std::string difficulty) {
	if (difficulty == "easy") {
		return 0;
	}
    if (difficulty == "normal") {
        return 1;
    } else if (difficulty == "hard") {
        return 2;
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

void Battle::convertBeatsToMilliseconds(std::vector<std::pair<float, BattleMode>> *mode_timings, float bpm_ratio) {
	for (int i = 0; i < mode_timings->size(); i++) {
		float time = mode_timings->at(i).first;
		float converted_timing = (time * 1000.f / bpm_ratio) - (note_travel_time * timing_offset) - 1.5f * 60.f / bpm_ratio;
		mode_timings->at(i).first = converted_timing;
	}
}

BattleMode Battle::convertStringToBattleMode(std::string mode_string) {
	if (mode_string == "back_and_forth") {
		return back_and_forth;
	}
	else if (mode_string == "beat_rush") {
		return beat_rush;
	}
	else if (mode_string == "unison") {
		return unison;
	}
	else {
		printf("Invalid mode in JSON; returning back_and_forth as default\n");
		return back_and_forth;
	}
}