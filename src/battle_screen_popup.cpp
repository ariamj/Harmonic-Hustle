#include "battle_screen.hpp"
#include "world_init.hpp"
#include <iostream>
#include <fstream>

void Battle::setReminderPopUp() {
	// add pop up background
	vec2 center = {gameInfo.width / 2.f, gameInfo.height / 2.f};
	Entity reminderBoxBottom = createBox(center, {gameInfo.width / 1.8f, gameInfo.height / 1.8f});
	Entity reminderBoxTop = createBox(center, {gameInfo.width / 1.8f - 20.f, gameInfo.height / 1.8f - 20.f});

	registry.colours.insert(reminderBoxBottom, Colour::theme_blue_2);
	registry.colours.insert(reminderBoxTop, Colour::theme_blue_1);

	registry.battleReminderPopUpParts.emplace(reminderBoxBottom);
	registry.battleReminderPopUpParts.emplace(reminderBoxTop);

	registry.screens.insert(reminderBoxBottom, Screen::BATTLE);
	registry.screens.insert(reminderBoxTop, Screen::BATTLE);

	in_reminder = true;

	// render entities as needed depending on level
	switch (gameInfo.curr_level) {
		case 1:
			addReminderPopUpPartsLevelOne();
			break;
		case 2:
			addReminderPopUpPartsLevelTwo();
			break;
		case 3:
			addReminderPopUpPartsLevelThree();
			break;
		default: // boss level
			addReminderPopUpPartsLevelBoss();
	}
};

// render entities as needed depending on difficulty, else use default
void Battle::addReminderPopUpPartsLevelOne() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			addDefaultReminderParts();
	}
}

// render entities as needed depending on difficulty, else use default
void Battle::addReminderPopUpPartsLevelTwo() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			addDefaultReminderParts();
	}
	
}

// render entities as needed depending on difficulty, else use default
void Battle::addReminderPopUpPartsLevelThree() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			addDefaultReminderParts();
	}
	
}

void Battle::addReminderPopUpPartsLevelBoss() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			addDefaultReminderParts();
	}
	
}

void Battle::addDefaultReminderParts() {
	float imgXPos = gameInfo.width / 2.f;
	float imgYPos = gameInfo.height / 2.f + 45.f;
	Entity aboveImg = createHelpImage(renderer, vec2(imgXPos - 100.f, imgYPos), TEXTURE_ASSET_ID::NOTE_EXAMPLE_ABOVE, Screen::BATTLE);
	Entity onImg = createHelpImage(renderer, vec2(imgXPos, imgYPos), TEXTURE_ASSET_ID::NOTE_EXAMPLE_ON, Screen::BATTLE);
	Entity hitImg = createHelpImage(renderer, vec2(imgXPos + 100.f, imgYPos),TEXTURE_ASSET_ID::NOTE_EXAMPLE_HIT, Screen::BATTLE);

	registry.battleReminderPopUpParts.emplace(aboveImg);
	registry.battleReminderPopUpParts.emplace(onImg);
	registry.battleReminderPopUpParts.emplace(hitImg);
}


void Battle::renderReminderText() {
	// render help text with simple instructions

	// render entities as needed depending on level
	switch (gameInfo.curr_level) {
		case 1:
			renderReminderTextLevelOne();
			break;
		case 2:
			renderReminderTextLevelTwo();
			break;
		case 3:
			renderReminderTextLevelThree();
			break;
		default: // boss level
			renderReminderTextLevelBoss();
	}
}

void Battle::renderReminderTextLevelOne() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			renderDefaultReminderText();
	}
}

void Battle::renderReminderTextLevelTwo() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			renderDefaultReminderText();
	}
}

void Battle::renderReminderTextLevelThree() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			renderDefaultReminderText();
	}
}

void Battle::renderReminderTextLevelBoss() {
	switch(gameInfo.curr_difficulty) {
		case 0:
		case 1:
		case 2:
		default:
			float reminderTextYPos = gameInfo.height / 3.7f;
			float reminderTextXPos = gameInfo.width / 2.f;
			createText("It's time for the boss battle!", vec2(reminderTextXPos, reminderTextYPos), 0.85f, Colour::red, glm::mat4(1.f), Screen::BATTLE, true);

			reminderTextYPos += 60.f;
			createText("time to prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, glm::mat4(1.f), Screen::BATTLE, true);

			reminderTextYPos += 45.f;
			createText("use keys", vec2(reminderTextXPos - 60.f, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

			// reminderTextYPos += 50.f;
			createText("D F J K", vec2(reminderTextXPos +  65.f, reminderTextYPos), 0.45f, Colour::red, glm::mat4(1.f), Screen::BATTLE, true);

			reminderTextYPos += 40.f;
			createText("to hit notes in the corresponding lanes", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

			reminderTextYPos += 40.f;
			createText("hit the notes as they pass the center of the line", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
			
			//// space for help images
			reminderTextYPos += 170.f;
			createText("remember to hold long notes till the end of the trail!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

			reminderTextYPos += 40.f;
			createText("to pass this level, get a score higher than ", vec2(reminderTextXPos - 30.f, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

			createText(std::to_string((int)score_threshold), vec2(reminderTextXPos + 320.f, reminderTextYPos), 0.45f, Colour::red, glm::mat4(1.f), Screen::BATTLE, true);

			reminderTextYPos += 55.f;
			createText("press space to start the battle", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, glm::mat4(1.f), Screen::BATTLE, true);

			// renderDefaultReminderText();
	}
}

void Battle::renderDefaultReminderText() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;
	createText("oh no You've hit an enemy!", vec2(reminderTextXPos, reminderTextYPos), 0.95f, Colour::red, glm::mat4(1.f), Screen::BATTLE, true);

	reminderTextYPos += 60.f;
	createText("time to prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, glm::mat4(1.f), Screen::BATTLE, true);

	reminderTextYPos += 45.f;
	createText("use keys", vec2(reminderTextXPos - 60.f, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

	// reminderTextYPos += 50.f;
	createText("D F J K", vec2(reminderTextXPos +  65.f, reminderTextYPos), 0.45f, Colour::red, glm::mat4(1.f), Screen::BATTLE, true);

	reminderTextYPos += 40.f;
	createText("to hit notes in the corresponding lanes", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

	reminderTextYPos += 40.f;
	createText("hit the notes as they pass the center of the line", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);
	
	//// space for help images
	reminderTextYPos += 170.f;
	createText("remember to hold long notes till the end of the trail!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

	reminderTextYPos += 40.f;
	createText("to pass this level, get a score higher than ", vec2(reminderTextXPos - 30.f, reminderTextYPos), 0.45f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

	createText(std::to_string((int)score_threshold), vec2(reminderTextXPos + 320.f, reminderTextYPos), 0.45f, Colour::red, glm::mat4(1.f), Screen::BATTLE, true);

	reminderTextYPos += 55.f;
	createText("press space to start the battle", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, glm::mat4(1.f), Screen::BATTLE, true);
}

void Battle::handle_exit_reminder() {
	in_reminder = false;
	just_exited_reminder = true;
	audio->pauseMusic();
	while (registry.battleReminderPopUpParts.entities.size() > 0)
		registry.remove_all_components_of(registry.battleReminderPopUpParts.entities.back());

};

///// GAME OVER POP UP METHODS---------------------

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

void Battle::renderGameOverText() {
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
	createText("Best Combo: " + std::to_string(max_combo), vec2(gameInfo.width / 2.f, gameInfo.height / 2.f + (spacing * 3.25)), 0.6f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

	// next instruction
	createText("...Press space to continue...", vec2(gameInfo.width/2.f, gameInfo.height/2.f + (spacing * 4.75)), 0.4f, Colour::black, glm::mat4(1.f), Screen::BATTLE, true);

	// notify savenext_note_spawn
	createText("Game saved", vec2(gameInfo.width / 2.f, gameInfo.height / 2.f - (spacing * 6)), 0.4f, Colour::green, glm::mat4(1.f), Screen::BATTLE, true);
	
}