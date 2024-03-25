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
bool Serializer::load_game() {
	Json::CharReaderBuilder rbuilder;
	return false;

}

// writer
// need to save: 
//		player: overworld position, current level
//		enemies: overworld positions, levels
bool Serializer::save_game() {
	Entity e = registry.players.entities[0];
	Motion& motion = registry.motions.get(e);
	Json::Value root;
	Json::StreamWriterBuilder wBuilder;
	
	root["player"]["level"] = registry.levels.get(*gameInfo.player_sprite).level;
	Json::Value subroot;
	subroot.append(motion.position.x);
	subroot.append(motion.position.y);
	
	root["player"]["position"] = subroot;
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

	// write string to file
	std::string filename = "save_" + std::to_string(0) + ".json";
	std::ofstream MyFile(saves_path(filename));

	// write to the file our game state;
	MyFile << document;

	// Close the file
	MyFile.close();

	
	return false;
}
