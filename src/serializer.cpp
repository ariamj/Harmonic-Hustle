#pragma once
#include "world_system.hpp"
#include <iostream>
#include <fstream>
#include "../ext/jsoncpp/json/json.h"


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
	}
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
		Entity e = registry.players.entities[0];
		Motion& motion = registry.motions.get(e);

		int lvl = root["player"]["level"].asInt();
		registry.levels.get(*gameInfo.player_sprite).level = lvl;
		gameInfo.curr_level = lvl;
		motion.position = vec2(root["player"]["position"][0].asFloat(), root["player"]["position"][1].asFloat());
		auto& enemiesData = gameInfo.existing_enemy_info;

		Json::Value enemies = root["enemies"];
		for (auto it = enemies.begin(); it != enemies.end(); ++it) {
			
			std::string enemyID = it.key().asString();

			Json::Value position = (*it)["position"];
			for (const auto& coord : position) {
				float enemy_x = coord[0].asFloat();
				float enemy_y = coord[1].asFloat();
				int enemy_lvl = coord[2].asInt();
				enemiesData.push_back({enemy_x, enemy_y, (float)enemy_lvl});
				printf("Enemy's (x, y, level): (%.2f, %.2f, %.2f)\n", enemy_x, enemy_y, (float) enemy_lvl);
			}
		}

		gameInfo.is_intro_finished = root["gameInfo"]["is_intro_finished"].asBool();
		gameInfo.is_boss_finished = root["gameInfo"]["is_boss_finished"].asBool();
		gameInfo.is_game_over_finished = root["gameInfo"]["is_game_over_finished"].asBool();
		gameInfo.gameIsOver = root["gameInfo"]["gameIsOver"].asBool();
		gameInfo.curr_difficulty = root["gameInfo"]["curr_difficulty"].asInt();
		gameInfo.curr_lives = root["gameInfo"]["curr_lives"].asInt();
		
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
	for (int i = 0; i < registry.enemies.size(); i++) {
		Entity e = registry.enemies.entities[i];
		const auto& m = registry.motions.get(e);
		const auto& pos = m.position;
		auto& level = registry.levels.get(e).level;
		std::string level_str = std::to_string(level);
		Json::Value enemyInfo;
		enemyInfo.append(pos.x);
		enemyInfo.append(pos.y);
		enemyInfo.append(level);
		root["enemies"][level_str]["position"].append(enemyInfo);

	}
	root["gameInfo"]["is_intro_finished"] = gameInfo.is_intro_finished;
	root["gameInfo"]["is_boss_finished"] = gameInfo.is_boss_finished;
	root["gameInfo"]["is_game_over_finished"] = gameInfo.is_game_over_finished;
	root["gameInfo"]["gameIsOver"] = gameInfo.gameIsOver;
	root["gameInfo"]["curr_difficulty"] = gameInfo.curr_difficulty;
	root["gameInfo"]["curr_lives"] = gameInfo.curr_lives;

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
