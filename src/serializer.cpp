#pragma once
#include "serializer.hpp"
#include "world_system.hpp"
#include <iostream>
#include <fstream>
#include "../ext/jsoncpp/json/json.h"


// load saved game state from save_<file number>.txt and return true if success
// if file dne, make such a file and return false
// saves to save_0 file
bool loadSave(int file_number) {
	std::string filename = "save_" + std::to_string(file_number) + ".txt";
	FILE* fptr;
	fptr = fopen(saves_path(filename).c_str(), "r");
	if (fptr != NULL) {
		// TODO: read game data from file
		if (fscanf(fptr, "%d", &gameInfo.curr_level) == 1) {
			printf("ok\n");
			fclose(fptr);
		}
		else printf("something wrong\n");

		return true;
	}
	else {
		std::ofstream MyFile(saves_path(filename));
		// write to the file our initial state;
		printf("Else\n");
		// Close the file
		MyFile.close();
	}
	return false;
}

Serializer::Serializer() {

}

// reader
// load game state by creating enemies and player according to saved data
bool Serializer::load_game() {
	Json::CharReaderBuilder rbuilder;
	std::string filename = "save_" + std::to_string(0) + ".json";
	std::ifstream file(saves_path(filename));
	if (!file.is_open()) {
		// make the missing file.
		printf("Couldn't open save file.\n");
		std::ofstream MyFile(saves_path(filename));
		printf("Created missing save file.\n");
		// Close the file
		MyFile.close();
		return false;
	}Json::Value root;
		Json::CharReaderBuilder builder;

	try {
		
		builder["collectComments"] = true;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, file, &root, &errs)) {
			Json::StreamWriterBuilder writerBuilder;
			std::string errsStr = Json::writeString(writerBuilder, errs);
			printf("Failed to parse JSON: %s\n", errsStr.c_str());
			return false;
		}
		file.close();
		Entity e = registry.players.entities[0];
		Motion& motion = registry.motions.get(e);

		int lvl = root["player"]["level"].asInt();
		registry.levels.get(*gameInfo.player_sprite).level = lvl;
		gameInfo.curr_level = lvl;
		motion.position = vec2(root["player"]["position"][0].asFloat(), root["player"]["position"][1].asFloat());
		//printf("Hi %s\n", f.c_str());
		//printf("fr: %f\n", fr);
		printf("Goodday\n");
		
	}
	catch (std::exception e) {
		printf("Exception reading!\n");
		return false;
	}

	return true;

}

// writer
// saves the game state: 
//		player: overworld position, current level
//		enemies: overworld positions, levels
bool Serializer::save_game() {
	Entity e = registry.players.entities[0];
	Motion& motion = registry.motions.get(e);
	Json::Value root;
	Json::StreamWriterBuilder wBuilder;
	
	root["player"]["level"] = registry.levels.get(*gameInfo.player_sprite).level;
	root["player"]["position"].append(motion.position.x);
	root["player"]["position"].append(motion.position.y);
	registry.list_all_components();
	int count = 0;
	for (int i = 0; i < registry.enemies.size(); i++) {
		Entity e = registry.enemies.entities[i];
		const auto& m = registry.motions.get(e);
		const auto& pos = m.position;
		auto& level = registry.levels.get(e).level;
		std::string level_str = std::to_string(level);
		Json::Value enemyPos;
		enemyPos.append(pos.x);
		enemyPos.append(pos.y);
		root["enemies"][level_str]["position"].append(enemyPos);

	}
	root["gameInfo"]["is_intro_finished"] = gameInfo.is_intro_finished;
	root["gameInfo"]["is_boss_finished"] = gameInfo.is_boss_finished;
	

	// write to string
	std::string document = Json::writeString(wBuilder, root);

	// write to save_0 file (include option for multiple saves in future?)
	std::string filename = "save_" + std::to_string(0) + ".json";
	std::ofstream MyFile(saves_path(filename));
	MyFile << document;

	// Close the file
	MyFile.close();

	return true;
}
