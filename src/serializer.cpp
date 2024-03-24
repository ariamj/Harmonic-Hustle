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
// need to save: player level, player and enemy positions in overworld
bool Serializer::save_game() {
	Json::Value root;
	Json::StreamWriterBuilder wBuilder;
	root["playerLevel"] = registry.levels.get(*gameInfo.player_sprite).level;
	root["playerPosition"] = "one";	
	registry.levels.get(*gameInfo.player_sprite).level = 4;
	gameInfo.curr_level = 4;

	Entity e = registry.players.entities[0];
	Motion& motion = registry.motions.get(e);

	// write to string
	std::string document = Json::writeString(wBuilder, root);

	// write string to file
	std::string filename = "save_" + std::to_string(0) + ".json";
	FILE* fptr;
	std::ofstream MyFile(saves_path(filename));

	// write to the file our game state;
	MyFile << document;

	// Close the file
	MyFile.close();


	
	return false;
}
