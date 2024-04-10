#include "battle_screen.hpp"
#include "world_init.hpp"
#include <iostream>
#include <fstream>

void Battle::setReminderPopUp() {
	// Now calling this function repeatedly; clear previous
	while (registry.battleReminderPopUpParts.entities.size() > 0)
		registry.remove_all_components_of(registry.battleReminderPopUpParts.entities.back());

	// add pop up background
	vec2 center = {gameInfo.width / 2.f, gameInfo.height / 2.f};
	Entity reminderBoxBottom = createBox(center, {gameInfo.width / 1.8f, gameInfo.height / 1.8f});
	Entity reminderBoxTop = createBox(center, {gameInfo.width / 1.8f - 20.f, gameInfo.height / 1.8f - 20.f});
	registry.foregrounds.emplace(reminderBoxTop);

	registry.colours.insert(reminderBoxBottom, Colour::theme_blue_2);
	registry.colours.insert(reminderBoxTop, Colour::theme_blue_1);

	registry.battleReminderPopUpParts.emplace(reminderBoxBottom);
	registry.battleReminderPopUpParts.emplace(reminderBoxTop);

	registry.screens.insert(reminderBoxBottom, Screen::BATTLE);
	registry.screens.insert(reminderBoxTop, Screen::BATTLE);

	in_reminder = true;

	// render specific pop up if enemy level > player level
	if (enemy_level > gameInfo.curr_level) {
		switch(popup_index) {
			case 0:
				addReminderPopUpPartsChallengeLevel();
				break;
			default:
				handle_exit_reminder();
				break;
		}
	} else {	
		// render entities as needed depending on level
		switch (enemy_level) {
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
				break;
		}
	}

	popup_index += 1;
};

// render entities as needed depending on difficulty, else use default
void Battle::addReminderPopUpPartsLevelOne() {
	switch(gameInfo.curr_difficulty) {
		case 0:
			// Nested switch for popups in sequence...
			switch (popup_index) {
				case 0:
					addExplanationBackAndForth();
					break;
				case 1:
					addDefaultReminderParts();
					break;
				default:
					handle_exit_reminder();
					break;
			}
			break;
		case 1:
			switch (popup_index) {
				case 0:
					addExplanationBackAndForth();
					break;
				case 1:
					addDefaultReminderParts();
					break;
				default:
					handle_exit_reminder();
					break;
			}
			break;
		case 2:
			switch (popup_index) {
				case 0:
					addExplanationBackAndForth();
					break;
				case 1:
					addDefaultReminderParts();
					break;
				default:
					handle_exit_reminder();
					break;
			}
			break;
		default:
			switch (popup_index) {
				case 0:
					addExplanationBackAndForth();
					break;
				default:
					handle_exit_reminder();
					break;
			}
	}
}

void Battle::addExplanationBackAndForth() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;

	Entity text1 = createText("You've challenged an enemy!", vec2(reminderTextXPos, reminderTextYPos), 0.8f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text2 = createText("Prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);

	reminderTextYPos += 45.f;
	Entity text3 = createText("In \"Back and Forth\" mode:", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::back_and_forth_colour, Screen::BATTLE, true, true);

	reminderTextYPos += 50.f;
	Entity text4 = createText("You will first hear a melody in the music (\"BACK\")", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text5 = createText("Correct timing will be based on that melody (\"FORTH\")", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text6 = createText("Particles will appear BLUE in colour", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 55.f;
	Entity continue_text = createText("press space to continue", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, Screen::BATTLE, true, true);
	
	registry.battleReminderPopUpParts.emplace(text1);
	registry.battleReminderPopUpParts.emplace(text2);
	registry.battleReminderPopUpParts.emplace(text3);
	registry.battleReminderPopUpParts.emplace(text4);
	registry.battleReminderPopUpParts.emplace(text5);
	registry.battleReminderPopUpParts.emplace(text6);	
	registry.battleReminderPopUpParts.emplace(continue_text);
}

void Battle::addExplanationBeatRush() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;

	Entity text1 = createText("You've challenged an enemy!", vec2(reminderTextXPos, reminderTextYPos), 0.8f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text2 = createText("Prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);

	reminderTextYPos += 45.f;
	Entity text3 = createText("In \"Beat Rush\" mode:", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::beat_rush_colour, Screen::BATTLE, true, true);

	reminderTextYPos += 50.f;
	Entity text4 = createText("Notes will spawn continuously, at a fixed rate", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text5 = createText("Unlike \"Back and Forth\", there won't be waiting time!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text6 = createText("Particles will appear RED in colour", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 80.f;
	Entity text7 = createText("Modes will change during the battle. Stay alert!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 55.f;
	Entity continue_text = createText("press space to continue", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, Screen::BATTLE, true, true);

	registry.battleReminderPopUpParts.emplace(text1);
	registry.battleReminderPopUpParts.emplace(text2);
	registry.battleReminderPopUpParts.emplace(text3);
	registry.battleReminderPopUpParts.emplace(text4);
	registry.battleReminderPopUpParts.emplace(text5);
	registry.battleReminderPopUpParts.emplace(text6);	
	registry.battleReminderPopUpParts.emplace(text7);	
	registry.battleReminderPopUpParts.emplace(continue_text);
}

void Battle::addExplanationUnison() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;

	Entity text1 = createText("You've challenged an enemy!", vec2(reminderTextXPos, reminderTextYPos), 0.8f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text2 = createText("Prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);

	reminderTextYPos += 45.f;
	Entity text3 = createText("In \"Unison\" mode:", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::unison_colour, Screen::BATTLE, true, true);

	reminderTextYPos += 50.f;
	Entity text4 = createText("Correct timing is at the same time as the melody!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text5 = createText("If you listen closely, you might notice... ", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);
	
	reminderTextYPos += 40.f;
	Entity text6 = createText("the same timing/rhythm has appeared previously!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text7 = createText("Particles will appear ORANGE in colour", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 55.f;
	Entity continue_text = createText("press space to continue", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, Screen::BATTLE, true, true);

	registry.battleReminderPopUpParts.emplace(text1);
	registry.battleReminderPopUpParts.emplace(text2);
	registry.battleReminderPopUpParts.emplace(text3);
	registry.battleReminderPopUpParts.emplace(text4);
	registry.battleReminderPopUpParts.emplace(text5);
	registry.battleReminderPopUpParts.emplace(text6);
	registry.battleReminderPopUpParts.emplace(text7);
	registry.battleReminderPopUpParts.emplace(continue_text);
}

// render entities as needed depending on difficulty, else use default
void Battle::addReminderPopUpPartsLevelTwo() {
	// Same across all difficulties
	switch (popup_index) {
		case 0:
			addExplanationBeatRush();
			break;
		case 1:
			addDefaultReminderParts();
			break;
		default:
			handle_exit_reminder();
			break;
	}
}

// render entities as needed depending on difficulty, else use default
void Battle::addReminderPopUpPartsLevelThree() {
	// Same across all difficulties
	switch (popup_index) {
		case 0:
			addExplanationUnison();
			break;
		case 1:
			addDefaultReminderParts();
			break;
		default:
			handle_exit_reminder();
			break;
	}
}

void Battle::addReminderPopUpPartsLevelBoss() {
	// Same across all difficulties
	switch(popup_index) {
		case 0:
			addExplanationBoss();
			break;
		case 1:
			addDefaultReminderPartsBoss();
			break;
		default:
			handle_exit_reminder();
			break;
	}
}

void Battle::addExplanationBoss() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;
	// testing update boss battle text
	Entity text1 = createText("It's time to fight the boss!", vec2(reminderTextXPos, reminderTextYPos), 0.85f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text2 = createText("Prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);

	reminderTextYPos += 65.f;
	Entity text6 = createText("Oh boy, here we go...", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 65.f;
	Entity text3 = createText("This fight will use all three modes!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 50.f;
	Entity text4 = createText("Watch out for multiple simultaneous notes!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 200.f;
	Entity text5 = createText("press space to continue", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, Screen::BATTLE, true, true);
	
	registry.battleReminderPopUpParts.emplace(text1);
	registry.battleReminderPopUpParts.emplace(text2);
	registry.battleReminderPopUpParts.emplace(text3);
	registry.battleReminderPopUpParts.emplace(text4);
	registry.battleReminderPopUpParts.emplace(text5);
	registry.battleReminderPopUpParts.emplace(text6);
}


// The only difference from normal level is the top text
void Battle::addDefaultReminderPartsBoss() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;
	Entity text1 = createText("It's time to fight the boss!", vec2(reminderTextXPos, reminderTextYPos), 0.8f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text2 = createText("Prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);

	reminderTextYPos += 45.f;
	Entity text3 = createText("use keys", vec2(reminderTextXPos - 60.f, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	// reminderTextYPos += 50.f;
	Entity text4 = createText("D F J K", vec2(reminderTextXPos +  65.f, reminderTextYPos), 0.45f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text5 = createText("to hit notes in the corresponding lanes", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text6 = createText("hit the notes as they pass the center of the line", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);
	
	// help images
	float imgXPos = gameInfo.width / 2.f;
	float imgYPos = gameInfo.height / 2.f + 45.f;
	Entity aboveImg = createHelpImage(renderer, vec2(imgXPos - 100.f, imgYPos), vec2(80, 100),TEXTURE_ASSET_ID::NOTE_EXAMPLE_ABOVE, Screen::BATTLE);
	Entity onImg = createHelpImage(renderer, vec2(imgXPos, imgYPos), vec2(80, 100), TEXTURE_ASSET_ID::NOTE_EXAMPLE_ON, Screen::BATTLE);
	Entity hitImg = createHelpImage(renderer, vec2(imgXPos + 100.f, imgYPos),vec2(80, 100), TEXTURE_ASSET_ID::NOTE_EXAMPLE_HIT, Screen::BATTLE);

	//// space for help images
	reminderTextYPos += 170.f;
	Entity text7 = createText("hold longer notes till the end of the trail!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text8 = createText("to pass this level, score higher than ", vec2(reminderTextXPos - 30.f, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	Entity text9 = createText(std::to_string((int)score_threshold), vec2(reminderTextXPos + 320.f, reminderTextYPos), 0.45f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 55.f;
	Entity text10 = createText("press space to Start", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, Screen::BATTLE, true, true);

	registry.battleReminderPopUpParts.emplace(aboveImg);
	registry.battleReminderPopUpParts.emplace(onImg);
	registry.battleReminderPopUpParts.emplace(hitImg);
	registry.battleReminderPopUpParts.emplace(text1);
	registry.battleReminderPopUpParts.emplace(text2);
	registry.battleReminderPopUpParts.emplace(text3);
	registry.battleReminderPopUpParts.emplace(text4);
	registry.battleReminderPopUpParts.emplace(text5);
	registry.battleReminderPopUpParts.emplace(text6);
	registry.battleReminderPopUpParts.emplace(text7);
	registry.battleReminderPopUpParts.emplace(text8);
	registry.battleReminderPopUpParts.emplace(text9);
	registry.battleReminderPopUpParts.emplace(text10);	
}

void Battle::addReminderPopUpPartsChallengeLevel() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;
	
	Entity text1 = createText("so you've challenged", vec2(reminderTextXPos, reminderTextYPos), 0.8f, Colour::red, Screen::BATTLE, true, true);
	
	reminderTextYPos += 40.f;
	Entity text2 = createText("a red enemy", vec2(reminderTextXPos, reminderTextYPos), 0.8f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 90.f;
	Entity text3 = createText("unfortunately that means you'll", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);
	
	reminderTextYPos += 45.f;
	Entity text4 = createText("get no battle tips here...", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);
	
	reminderTextYPos += 70.f;
	// Entity text5 = createText("hope you know what you're doing", vec2(reminderTextXPos, reminderTextYPos), 0.35f, Colour::theme_blue_3, Screen::BATTLE, true, true);

	reminderTextYPos += 50.f;
	Entity text6 = createText("good luck!", vec2(reminderTextXPos, reminderTextYPos), 1.f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 140.f;
	Entity text10 = createText("press space to Start", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, Screen::BATTLE, true, true);


	registry.battleReminderPopUpParts.emplace(text1);
	registry.battleReminderPopUpParts.emplace(text2);
	registry.battleReminderPopUpParts.emplace(text3);
	registry.battleReminderPopUpParts.emplace(text4);
	// registry.battleReminderPopUpParts.emplace(text5);
	registry.battleReminderPopUpParts.emplace(text6);
	registry.battleReminderPopUpParts.emplace(text10);
};

void Battle::addDefaultReminderParts() {
	float reminderTextYPos = gameInfo.height / 3.7f;
	float reminderTextXPos = gameInfo.width / 2.f;
	Entity text1 = createText("You've challenged an enemy!", vec2(reminderTextXPos, reminderTextYPos), 0.8f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 60.f;
	Entity text2 = createText("Prepare for battle", vec2(reminderTextXPos, reminderTextYPos), 0.65f, Colour::theme_blue_3, Screen::BATTLE, true, true);

	reminderTextYPos += 45.f;
	Entity text3 = createText("use keys", vec2(reminderTextXPos - 60.f, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	// reminderTextYPos += 50.f;
	Entity text4 = createText("D F J K", vec2(reminderTextXPos +  65.f, reminderTextYPos), 0.45f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text5 = createText("to hit notes in the corresponding lanes", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text6 = createText("hit the notes as they pass the center of the line", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);
	
	// help images
	float imgXPos = gameInfo.width / 2.f;
	float imgYPos = gameInfo.height / 2.f + 45.f;
	Entity aboveImg = createHelpImage(renderer, vec2(imgXPos - 100.f, imgYPos), vec2(80, 100),TEXTURE_ASSET_ID::NOTE_EXAMPLE_ABOVE, Screen::BATTLE);
	Entity onImg = createHelpImage(renderer, vec2(imgXPos, imgYPos), vec2(80, 100), TEXTURE_ASSET_ID::NOTE_EXAMPLE_ON, Screen::BATTLE);
	Entity hitImg = createHelpImage(renderer, vec2(imgXPos + 100.f, imgYPos),vec2(80, 100), TEXTURE_ASSET_ID::NOTE_EXAMPLE_HIT, Screen::BATTLE);

	//// space for help images
	reminderTextYPos += 170.f;
	Entity text7 = createText("hold longer notes until the end of the trail!", vec2(reminderTextXPos, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	reminderTextYPos += 40.f;
	Entity text8 = createText("to pass this level, get a score higher than ", vec2(reminderTextXPos - 30.f, reminderTextYPos), 0.45f, Colour::black, Screen::BATTLE, true, true);

	Entity text9 = createText(std::to_string((int)score_threshold), vec2(reminderTextXPos + 320.f, reminderTextYPos), 0.45f, Colour::red, Screen::BATTLE, true, true);

	reminderTextYPos += 55.f;
	Entity text10 = createText("press space to Start", vec2(reminderTextXPos, reminderTextYPos), 0.7f, Colour::dark_green, Screen::BATTLE, true, true);

	registry.battleReminderPopUpParts.emplace(aboveImg);
	registry.battleReminderPopUpParts.emplace(onImg);
	registry.battleReminderPopUpParts.emplace(hitImg);
	registry.battleReminderPopUpParts.emplace(text1);
	registry.battleReminderPopUpParts.emplace(text2);
	registry.battleReminderPopUpParts.emplace(text3);
	registry.battleReminderPopUpParts.emplace(text4);
	registry.battleReminderPopUpParts.emplace(text5);
	registry.battleReminderPopUpParts.emplace(text6);
	registry.battleReminderPopUpParts.emplace(text7);
	registry.battleReminderPopUpParts.emplace(text8);
	registry.battleReminderPopUpParts.emplace(text9);
	registry.battleReminderPopUpParts.emplace(text10);
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
		registry.foregrounds.emplace(gameOverPopUpOverlay);
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
		createText("Congratulations!!!", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 4)), 0.75f, Colour::black, Screen::BATTLE);
		createText("Enemy has been defeated", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 3)), 0.75f, Colour::black, Screen::BATTLE);
	} else {
		if (gameInfo.curr_lives == 0) {
			createText("You have been defeated!!!", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 4)), 0.75f, Colour::black, Screen::BATTLE);
			createText("And you've run out of lives :(", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 3)), 0.75f, Colour::red, Screen::BATTLE);
		} else if ((registry.enemies.entities.size() == 0 ) && (gameInfo.curr_lives != 0)) {
			createText("You have been defeated!!!", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 4)), 0.75f, Colour::black, Screen::BATTLE);
			createText("And you've run out of chances", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 3)), 0.73f, Colour::red, Screen::BATTLE);
			createText("to advance", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 2)), 0.73f, Colour::red, Screen::BATTLE);
		} else {
			createText("You have been defeated!!!", vec2(gameInfo.width/2.f, gameInfo.height/2.f - (spacing * 3.5)), 0.75f, Colour::black, Screen::BATTLE);
		}
	}
	createText("Score: " + std::to_string((int)score), vec2(gameInfo.width/2.f - (score_x_spacing * 2), gameInfo.height/2.f - spacing), 0.75f, Colour::black, Screen::BATTLE);
	createText("Enemy: " + std::to_string((int)score_threshold), vec2(gameInfo.width/2.f + (score_x_spacing * 2), gameInfo.height/2.f - spacing), 0.75f, Colour::black, Screen::BATTLE);
	
	// Scoring
	float scoring_text_size = 0.5f;
	createText("Perfect", vec2(gameInfo.width/2.f -( score_x_spacing * 3), gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_purple, Screen::BATTLE);
	createText("Good", vec2(gameInfo.width/2.f - score_x_spacing, gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_green, Screen::BATTLE);
	createText("Alright", vec2(gameInfo.width/2.f + score_x_spacing, gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_yellow, Screen::BATTLE);
	createText("Missed", vec2(gameInfo.width/2.f + (score_x_spacing * 3), gameInfo.height/2.f + spacing), scoring_text_size, Colour::dark_red, Screen::BATTLE);

	createText(std::to_string(perfect_counter), vec2(gameInfo.width/2.f -( score_x_spacing * 3), gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_purple, Screen::BATTLE);
	createText(std::to_string(good_counter), vec2(gameInfo.width/2.f - score_x_spacing, gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_green, Screen::BATTLE);
	createText(std::to_string(alright_counter), vec2(gameInfo.width/2.f + score_x_spacing, gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_yellow, Screen::BATTLE);
	createText(std::to_string(missed_counter), vec2(gameInfo.width/2.f + (score_x_spacing * 3), gameInfo.height/2.f + (spacing*2)), scoring_text_size, Colour::dark_red, Screen::BATTLE);

	// Combo
	createText("Best Combo: " + std::to_string(max_combo), vec2(gameInfo.width / 2.f, gameInfo.height / 2.f + (spacing * 3.25)), 0.6f, Colour::black, Screen::BATTLE);

	// next instruction
	createText("...Press space to continue...", vec2(gameInfo.width/2.f, gameInfo.height/2.f + (spacing * 4.1f)), 0.4f, Colour::black, Screen::BATTLE);

	// notify savenext_note_spawn
	createText("Game saved", vec2(gameInfo.width / 2.f, gameInfo.height / 2.f - (spacing * 6)), 0.4f, Colour::green, Screen::BATTLE);
	
}