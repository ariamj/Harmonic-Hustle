#include "tutorial_screen.hpp"

#include <sstream>
#include <iostream>
#include "world_init.hpp"

std::string getFramesString();

Tutorial::Tutorial() {

};

Tutorial::~Tutorial() {

};

void Tutorial::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;

    // create buttons here, don't add to screen yet
}

// remove existing entities and create entities for the needed part
// doesn't create any for last enum which is just the num of parts
void Tutorial::init_parts(TutorialPart part) {

    while (registry.tutorialParts.entities.size() > 0)
		registry.remove_all_components_of(registry.tutorialParts.entities.back());

    if (tutorial_progress == TutorialPart::INTRO) {
        initIntroParts();
    } else if (tutorial_progress == TutorialPart::BATTLE_EXPLAIN) {
        initBattleExplainParts();
    } else if (tutorial_progress == TutorialPart::ADVANCING_EXPLAIN) {
        initAdvancingExplaingParts();
    } else if (tutorial_progress == TutorialPart::CHOOSE_DIFFICULTY) {
        initChooseDifficultyParts();
    }
    else if (tutorial_progress == TutorialPart::ADJUST_TIMING) {
        initAdjustNoteTimingParts();
    }

    // vec2 center = {gameInfo.width / 2.f, gameInfo.height / 2.f};
    // Entity testing = createHelpImage(renderer, center, vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::PLAYER_WALK_F1, Screen::TUTORIAL);

    // Entity testing = createCSPlayer(renderer, center, TEXTURE_ASSET_ID::NOTE_EXAMPLE_ABOVE, Screen::TUTORIAL);
};

bool Tutorial::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
    title_ss << "Harmonic Hustle";
    title_ss << " --- Tutorial ";
    glfwSetWindowTitle(window, title_ss.str().c_str());
    // createText("Tutorial", vec2(gameInfo.width / 2.f, gameInfo.height / 20.f), 1.3f, Colour::white, Screen::TUTORIAL );

    if (tutorial_progress == TutorialPart::CHOOSE_DIFFICULTY) {
        // Render all button texts
        for (Entity entity : registry.boxButtons.entities) {
            if (registry.screens.get(entity) == Screen::TUTORIAL) {
                BoxButton btn = registry.boxButtons.get(entity);
                Motion motion = registry.motions.get(entity);
                std::string text = registry.boxButtons.get(entity).text;
                vec3 text_colour = btn.text_colour;

                // Hover effect
                // NOTE: if lag happens, comment this part out
                if (text == "X" && mouse_area == in_x_btn) {
                    text_colour = Colour::dark_red;
                }
                int difficulty = gameInfo.curr_difficulty;
                if ((text == "easy" && mouse_area == in_easy_btn && difficulty != 0) || (text == "normal" && mouse_area == in_normal_btn && difficulty !=1) || (text == "hard" && mouse_area == in_hard_btn && difficulty !=2)) {
                    text_colour = Colour::white;
                }
                if (text == "easy" && gameInfo.curr_difficulty == 0) text_colour = Colour::green; 
                if (text == "normal" && gameInfo.curr_difficulty == 1) text_colour = Colour::dark_blue;
                if (text == "hard" && gameInfo.curr_difficulty == 2) text_colour = Colour::red;
                createText(text, motion.position, btn.text_scale, text_colour, Screen::TUTORIAL, true, false);
            }
        }
    } else  if (tutorial_progress == TutorialPart::ADJUST_TIMING) {
        // Render all button texts
        for (Entity entity : registry.boxButtons.entities) {
            if (registry.screens.get(entity) == Screen::TUTORIAL) {
                BoxButton btn = registry.boxButtons.get(entity);
                Motion motion = registry.motions.get(entity);
                std::string text = registry.boxButtons.get(entity).text;
                vec3 text_colour = btn.text_colour;
                if (text == "X" && mouse_area == in_x_btn) {
                    text_colour = Colour::dark_red;
                }
                if (text == "-")  motion.position.y += 9.0f; // adjust minus symbol to look centered
                // Hover effect
                // NOTE: if lag happens, comment this part out
                if (text == "-" && mouse_area == in_decrease_frame_btn) {
                    text_colour = Colour::red;
                }
                else if (text == "+" && mouse_area == in_increase_frame_btn) {
                    text_colour = Colour::green;
                }
                createText(text, motion.position, btn.text_scale, text_colour, Screen::TUTORIAL, true, false);
            }
        }
        registry.texts.get(curr_timing_text).text = getFramesString();
    }
    // createText("Press space to continue", vec2(gameInfo.width/2.f, gameInfo.height * 7.5f / 8.f), 0.9f, Colour::white, Screen::TUTORIAL);
    return true;
}

void Tutorial::initIntroParts() {
    // vec2 spriteExPos = {gameInfo.width / 10.f, gameInfo.height / 5.f};
    // Entity testing = createHelpImage(renderer, spriteExPos, vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::PLAYER_WALK_F1, Screen::TUTORIAL);
    
    vec2 spriteExPos = {gameInfo.width * 8.5f / 10.f, gameInfo.height * 1.3f / 6.f};
    Entity playerSpriteExample = createHelpImage(renderer, spriteExPos, vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::PLAYER_WALK_F1, Screen::TUTORIAL);
    
    registry.tutorialParts.emplace(playerSpriteExample);

    Entity currEnemyExample = createHelpImage(renderer, vec2(gameInfo.width * 1.5f / 10.f, gameInfo.height * 2.7f / 6.f),
            vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::ENEMY_GUITAR, Screen::TUTORIAL);

    registry.tutorialParts.emplace(currEnemyExample);

    Entity redEnemyExample = createHelpImage(renderer, vec2(gameInfo.width * 8.5f / 10.f, gameInfo.height * 3.8f / 6.f),
            vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::ENEMY_MIC, Screen::TUTORIAL);

    registry.colours.insert(redEnemyExample, Colour::red_enemy_tint);

    registry.tutorialParts.emplace(redEnemyExample);

    ////////// ADD TEXT

    Entity titleText = createText("Tutorial", vec2(gameInfo.width / 2.f, gameInfo.height / 20.f), 1.3f, Colour::white, Screen::TUTORIAL, true, true );
    registry.tutorialParts.emplace(titleText);

    vec2 spriteExPos2 = spriteExPos - vec2{0, PLAYER_HEIGHT / 2.f + 10.f};
    Entity text1 = createText("lvl 1", spriteExPos2, 0.5f, Colour::green, Screen::TUTORIAL, true, true);

    vec2 enemyExPostion = vec2(gameInfo.width * 1.5f / 10.f, gameInfo.height * 2.7f / 6.f - PLAYER_HEIGHT / 2.f - 10.f);
    Entity text2 = createText("lvl 1", enemyExPostion, 0.5f, Colour::white, Screen::TUTORIAL, true, true);

    vec2 redEnemyExPosition = vec2(gameInfo.width * 8.5f / 10.f, gameInfo.height * 3.8f / 6.f - PLAYER_HEIGHT / 2.f - 10.f);
    Entity text3 = createText("lvl 2", redEnemyExPosition, 0.5f, Colour::red, Screen::TUTORIAL, true, true);

    float currXPos = gameInfo.width * 4.f / 10.f;
    float currYPos = gameInfo.height * 1.f/ 7.f;

    Entity text4 = createText("This is you! You're currently level 1. To win this game,", vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    currYPos += 30.f;
    Entity text5 = createText("you'll have to level up to be strong enough to defeat the boss.", vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    currYPos += 50.f;
    Entity text6 = createText("On spawn, you'll be in the overworld surrounded by enemies.", vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    currYPos += 40.f;
    Entity text7 = createText("W A S D", vec2(gameInfo.width * 2.5f / 10.f, currYPos), 0.65f, Colour::green, Screen::TUTORIAL, true, true);

    Entity text8 = createText("keys to move around", vec2(gameInfo.width * 4.5f / 10.f, currYPos), 0.65f, Colour::off_white, Screen::TUTORIAL, true, true);

    ///////////// right aligned
    float rightXPos = gameInfo.width * 6.f / 10.f;
    currYPos += 110.f;
    Entity text9 = createText("On spawn, you'll be in the overworld surrounded by enemies.", vec2(rightXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    currYPos += 40.f;
    Entity text10 = createText("Collide and ", vec2(rightXPos - 270.f, currYPos), 0.65f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text11 = createText("defeat same level enemies", 
        vec2(rightXPos + 150.f, currYPos), 0.65f, Colour::red, Screen::TUTORIAL, true, true);

    currYPos += 40.f;
    Entity text12 = createText("They'll have a white level tag", vec2(rightXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    ////////left aligned
    // currYPos += 170.f;
    currYPos += 110.f;
    Entity text13 = createText("run away", vec2(currXPos - 190.f, currYPos), 0.65f, Colour::red, Screen::TUTORIAL, true, true);
    Entity text14 = createText("from red enemies!", vec2(currXPos + 100.f, currYPos), 0.65f, Colour::off_white, Screen::TUTORIAL, true, true);

    currYPos += 40.f;
    Entity text15 = createText("they have a higher level than you.", vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);
    currYPos += 30.f;
    // TODO -> update if we rework the red enemy collision flow
    // Entity text16 = createText("if you hit them, you'll have to restart the game!", vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);
    
    Entity text16 = createText("if you hit them, you'll enter a", vec2(currXPos - 150.f, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text17 = createText("challenge battle", vec2(currXPos + 245.f, currYPos), 0.5f, Colour::red, Screen::TUTORIAL, true, true);
    currYPos += 30.f;
    Entity text18 = createText("you win, and you can skip the lower levels", vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);
    
    float centerX = gameInfo.width / 2.f;
    currYPos += 100.f;
    Entity text19 = createText("You start with", vec2(centerX - 80.f, currYPos), 0.65f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text20 = createText("3 lives", vec2(centerX + 160.f, currYPos), 0.65f, Colour::green, Screen::TUTORIAL, true, true);

    currYPos += 30.f;
    Entity text21 = createText("if you lose a challenge battle, you'll lose a life!", vec2(centerX, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);
    currYPos += 50.f;
    Entity text22 = createText("no more lives = game over", vec2(centerX, currYPos), 0.65f, Colour::red, Screen::TUTORIAL, true, true);


    registry.tutorialParts.emplace(text1);
    registry.tutorialParts.emplace(text2);
    registry.tutorialParts.emplace(text3);
    registry.tutorialParts.emplace(text4);
    registry.tutorialParts.emplace(text5);
    registry.tutorialParts.emplace(text6);
    registry.tutorialParts.emplace(text7);
    registry.tutorialParts.emplace(text8);
    registry.tutorialParts.emplace(text9);
    registry.tutorialParts.emplace(text10);
    registry.tutorialParts.emplace(text11);
    registry.tutorialParts.emplace(text12);
    registry.tutorialParts.emplace(text13);
    registry.tutorialParts.emplace(text14);
    registry.tutorialParts.emplace(text15);
    registry.tutorialParts.emplace(text16);
    registry.tutorialParts.emplace(text17);
    registry.tutorialParts.emplace(text18);
    registry.tutorialParts.emplace(text19);
    registry.tutorialParts.emplace(text20);
    registry.tutorialParts.emplace(text21);
    registry.tutorialParts.emplace(text22);

    Entity contText = createText("Press space to continue", vec2(gameInfo.width/2.f, gameInfo.height * 7.5f / 8.f), 0.9f, Colour::white, Screen::TUTORIAL, true, true);
    registry.tutorialParts.emplace(contText);

}

void Tutorial::initBattleExplainParts() {
    vec2 spriteExPos = {gameInfo.width * 1.5f / 10.f, gameInfo.height * 0.7f / 6.f};
    Entity playerSpriteExample = createHelpImage(renderer, spriteExPos, vec2(PLAYER_WIDTH, PLAYER_HEIGHT) * vec2(0.7), TEXTURE_ASSET_ID::ENEMY_GUITAR, Screen::TUTORIAL);
    
    registry.tutorialParts.emplace(playerSpriteExample);

    // ADDING TEXT

    vec2 spriteExPos2 = {gameInfo.width * 1.5f / 10.f, gameInfo.height * 0.7f / 6.f - PLAYER_HEIGHT * 0.7f / 2.f - 7.f };
    Entity text1 = createText("lvl 1", spriteExPos2, 0.5f * 0.7f, Colour::white, Screen::TUTORIAL, true, true);

    float rightXPos = gameInfo.width * 5.5f / 10.f;
    float currY = gameInfo.height / 12.f;
    Entity text2 = createText("If you hit one of these guys, you'll enter a battle!", vec2(rightXPos, currY), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    currY += 50.f;
    float centerX = gameInfo.width / 2.f;
    Entity text3 = createText("There are 2 different types of notes in battle", vec2(rightXPos, currY), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    // note examples
    currY += 170.f;
    Entity text4 = createText("tap notes", vec2(gameInfo.width * 1.7 / 8.f, currY), 0.7f, Colour::green, Screen::TUTORIAL, true, true);

    Entity text5 = createText("hold notes", vec2(gameInfo.width * 5.2f / 8.f, currY), 0.7f, Colour::green, Screen::TUTORIAL, true, true);

    // note images
    Entity tapNoteImage = createHelpImage(renderer, vec2(gameInfo.width * 3.2f/8.f, currY), vec2(90.f, 140.f), TEXTURE_ASSET_ID::TAP_NOTE_EXAMPLE, Screen::TUTORIAL);
    Entity holdNoteImage = createHelpImage(renderer, vec2(gameInfo.width * 6.7f/8.f, currY), vec2(80.f, 240.f), TEXTURE_ASSET_ID::HOLD_NOTE_EXAMPLE, Screen::TUTORIAL);

    currY += 170.f;
    Entity text6 = createText("to hit notes in the right lanes, use", vec2(centerX - 80.f, currY), 0.65f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text7 = createText("D F J K", vec2(centerX + 380.f, currY), 0.65f, Colour::red, Screen::TUTORIAL, true, true);

    // judgement lines img
    currY += 130.f;
    Entity judgementLinesImage = createHelpImage(renderer, vec2(centerX, currY), vec2(410.f, 170.f), TEXTURE_ASSET_ID::JUDGEMENT_LINES_EXAMPLE, Screen::TUTORIAL);

    // more text
    currY += 130.f;
    Entity text8 = createText("hit notes as they pass the", vec2(centerX - 90.f, currY), 0.7f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text9 = createText("center!", vec2(centerX + 310.f, currY), 0.7f, Colour::green, Screen::TUTORIAL, true, true);

    currY += 40.f;
    Entity text10 = createText("the closer you are, the higher the score", vec2(centerX, currY), 0.5f, Colour::off_white, Screen::TUTORIAL, true, true);

    currY += 70.f;
    Entity text11 = createText("to win,", vec2(centerX - 510.f, currY), 0.7f, Colour::green, Screen::TUTORIAL, true, true);
    Entity text12 = createText("get above the score threshold in each level!", vec2(centerX + 90.f, currY), 0.7f, Colour::off_white, Screen::TUTORIAL, true, true);

    registry.tutorialParts.emplace(text1);
    registry.tutorialParts.emplace(text2);
    registry.tutorialParts.emplace(text3);
    registry.tutorialParts.emplace(text4);
    registry.tutorialParts.emplace(text5);
    registry.tutorialParts.emplace(tapNoteImage);
    registry.tutorialParts.emplace(holdNoteImage);
    registry.tutorialParts.emplace(text6);
    registry.tutorialParts.emplace(text7);
    registry.tutorialParts.emplace(judgementLinesImage);
    registry.tutorialParts.emplace(text8);
    registry.tutorialParts.emplace(text9);
    registry.tutorialParts.emplace(text10);
    registry.tutorialParts.emplace(text11);
    registry.tutorialParts.emplace(text12);

    Entity contText = createText("Press space to continue", vec2(gameInfo.width/2.f, gameInfo.height * 7.5f / 8.f), 0.9f, Colour::white, Screen::TUTORIAL, true, true);
    registry.tutorialParts.emplace(contText);
}

void Tutorial::initAdvancingExplaingParts() {
    // Entity text1 = createText("TESTING 3", vec2(gameInfo.width/2.f, gameInfo.height / 2.f), 2.f, Colour::white, Screen::TUTORIAL, true, true);
    // registry.tutorialParts.emplace(text1);
    float currY = gameInfo.height / 12.f;
    float centerX = gameInfo.width / 2.f;

    Entity pauseTipText = createText("Tip: if you ever need to pause or check keys, click ESC!", vec2(centerX, currY), 0.7f, Colour::theme_blue_2 + vec3(0.3), Screen::TUTORIAL, true, true);
    registry.tutorialParts.emplace(pauseTipText);

    currY += 120.f;
    Entity text1 = createText("Each level will have", vec2(centerX - 90.f, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text2 = createText("3 enemies", vec2(centerX + 220.f, currY), 0.6f, Colour::red, Screen::TUTORIAL, true, true);

    currY += 50.f;
    Entity text3 = createText("You'll only have to defeat one to advance", vec2(centerX, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    currY += 40.f;
    Entity text4 = createText("But don't lose, cause you only get", vec2(centerX - 105.f, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text5 = createText("3 chances!", vec2(centerX + 345.f, currY), 0.6f, Colour::red, Screen::TUTORIAL, true, true);

    // explain boss
    float leftTextPos = gameInfo.width * 1.1f/ 3.f;

    currY += 190.f;
    Entity bossExampleImg = createHelpImage(renderer, vec2(gameInfo.width * 7.5f / 10.f, currY + 10.f), vec2(PORTRAIT_WIDTH, PORTRAIT_HEIGHT) * vec2(0.6), TEXTURE_ASSET_ID::BATTLEBOSS, Screen::TUTORIAL);
    registry.tutorialParts.emplace(bossExampleImg);
    
    Entity text6 = createText("After level 3, you'll encounter the boss", vec2(leftTextPos, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    currY += 40.f;
    Entity text7 = createText("To win the game,", vec2(leftTextPos - 165.f, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text8 = createText("defeat the boss!", vec2(leftTextPos + 165.f, currY), 0.6f, Colour::green, Screen::TUTORIAL, true, true);

    currY += 190.f;
    Entity text9 = createText("If you lose against the boss, you'll enter a", vec2(centerX - 100.f, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    Entity text10 = createText("rematch", vec2(centerX + 420.f, currY), 0.6f, Colour::red, Screen::TUTORIAL, true, true);

    currY += 40.f;
    Entity text11 = createText("You'll rematch infinitely until you beat the game", vec2(centerX, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    currY += 40.f;
    Entity text12 = createText("so good luck!", vec2(centerX, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);

    registry.tutorialParts.emplace(text1);
    registry.tutorialParts.emplace(text2);
    registry.tutorialParts.emplace(text3);
    registry.tutorialParts.emplace(text4);
    registry.tutorialParts.emplace(text5);
    registry.tutorialParts.emplace(text6);
    registry.tutorialParts.emplace(text7);
    registry.tutorialParts.emplace(text8);
    registry.tutorialParts.emplace(text9);
    registry.tutorialParts.emplace(text10);
    registry.tutorialParts.emplace(text11);
    registry.tutorialParts.emplace(text12);

    Entity contText = createText("Press space to continue", vec2(gameInfo.width/2.f, gameInfo.height * 7.5f / 8.f), 0.9f, Colour::white, Screen::TUTORIAL, true, true);
    registry.tutorialParts.emplace(contText);
}

void Tutorial::initChooseDifficultyParts() {
    // add x button at top right
    if (gameInfo.in_options) {
        vec2 buttonPos = vec2(gameInfo.width - 100.f, 100.f);
        vec2 buttonSize = vec2(100.f, 100.f);
        Entity x_btn_shadow = createBox(buttonPos + vec2(10.f, 10.f), buttonSize);
        registry.screens.insert(x_btn_shadow, Screen::TUTORIAL);
	    registry.colours.insert(x_btn_shadow, Colour::theme_blue_2 - vec3(0.2));
    

        x_btn = createButton("X", buttonPos, 1.5f, buttonSize, Colour::red , Colour::theme_blue_1, Screen::TUTORIAL);
        registry.tutorialParts.emplace(x_btn);
        registry.tutorialParts.emplace(x_btn_shadow);
    }

    // Entity text1 = createText("TESTING 3", vec2(gameInfo.width/2.f, gameInfo.height / 2.f), 2.f, Colour::white, Screen::TUTORIAL, true, true);
    // registry.tutorialParts.emplace(text1);
    float currY = gameInfo.height / 15.f;
    float centerX = gameInfo.width / 2.f;

    // explain difficulty
    currY += 100.f;
    Entity text9 = createText("Please choose a difficulty level", vec2(centerX, currY), 0.7f, Colour::off_white, Screen::TUTORIAL, true, true);
    currY += 50.f;
    Entity text10 = createText("difficulties indicate freqeuncy of note spawns in battle", vec2(centerX, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    
    registry.tutorialParts.emplace(text9);
    registry.tutorialParts.emplace(text10);

    // float buttonX = gameInfo.width / 6.f;
    float buttonX = gameInfo.width / 2.f;
    currY += 150.f;
	vec2 shadow_pos = vec2(buttonX, currY) + vec2(10.f, 10.f);

    vec2 buttonSize = vec2(gameInfo.width / 5.f, gameInfo.height / 10.f);

    // create buttons
    Entity easy_shadow = createBox(shadow_pos, buttonSize);
	registry.screens.insert(easy_shadow, Screen::TUTORIAL);
	registry.colours.insert(easy_shadow, Colour::theme_blue_3);
	easy_btn = createButton("easy", vec2(buttonX, currY), 1.0f, buttonSize, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::TUTORIAL);
	
    // buttonX = gameInfo.width / 2.f;
    currY += 180.f;
    shadow_pos = vec2(buttonX, currY) + vec2(10.f, 10.f);

    Entity normal_shadow = createBox(shadow_pos, buttonSize);
	registry.screens.insert(normal_shadow, Screen::TUTORIAL);
	registry.colours.insert(normal_shadow, Colour::theme_blue_3);
	normal_btn = createButton("normal", vec2(buttonX, currY), 1.0f, buttonSize, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::TUTORIAL);
	
    // buttonX = gameInfo.width * 5.f / 6.f;
    currY += 180.f;
    shadow_pos = vec2(buttonX, currY) + vec2(10.f, 10.f);

    Entity hard_shadow = createBox(shadow_pos, buttonSize);
	registry.screens.insert(hard_shadow, Screen::TUTORIAL);
	registry.colours.insert(hard_shadow, Colour::theme_blue_3);
	hard_btn = createButton("hard", vec2(buttonX, currY), 1.0f, buttonSize, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::TUTORIAL);
	
    if (!gameInfo.in_options) {
        currY += 170.f;
        Entity contText = createText("Press space to continue", vec2(gameInfo.width / 2.f, gameInfo.height * 7.5f / 8.f), 0.9f, Colour::white, Screen::TUTORIAL, true, true);
        registry.tutorialParts.emplace(contText);
    }

    registry.tutorialParts.emplace(hard_shadow);
    registry.tutorialParts.emplace(hard_btn);
    registry.tutorialParts.emplace(normal_shadow);
    registry.tutorialParts.emplace(normal_btn);
    registry.tutorialParts.emplace(easy_shadow);
    registry.tutorialParts.emplace(easy_btn);
}

std::string getFramesString() {
    std::ostringstream frames_str;
    frames_str.precision(2);
    frames_str << gameInfo.frames_adjustment;
    return frames_str.str();
}

void Tutorial::initAdjustNoteTimingParts()
{
    // add x button at top right
    if (gameInfo.in_options) {
        vec2 buttonPos = vec2(gameInfo.width - 100.f, 100.f);
        vec2 buttonSize = vec2(100.f, 100.f);
        Entity x_btn_shadow = createBox(buttonPos + vec2(10.f, 10.f), buttonSize);
        registry.screens.insert(x_btn_shadow, Screen::TUTORIAL);
	    registry.colours.insert(x_btn_shadow, Colour::theme_blue_2 - vec3(0.2));
    
        x_btn = createButton("X", buttonPos, 1.5f, buttonSize, Colour::red , Colour::theme_blue_1, Screen::TUTORIAL);
        registry.tutorialParts.emplace(x_btn);
        registry.tutorialParts.emplace(x_btn_shadow);
    }
    float currY = gameInfo.height / 8.f;
    float centerX = gameInfo.width / 2.f;
    float centerY = gameInfo.height / 2.f;
    float shift = 320.f;

    // explain difficulty
    currY += 100.f;
    Entity text0 = createText("Adjust timing of notes", vec2(centerX, currY), 0.7f, Colour::off_white, Screen::TUTORIAL, true, true);
    currY += 50.f;
    Entity text1 = createText("Timing of notes is measured in portions of frames", vec2(centerX, currY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    
    
    Entity adjustment_text = curr_timing_text = createText("Frame Adjustment: ", vec2(centerX - 50.f, centerY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);
    curr_timing_text = createText(getFramesString(), vec2(centerX + 190.f, centerY), 0.6f, Colour::off_white, Screen::TUTORIAL, true, true);

    registry.tutorialParts.emplace(text0);
    registry.tutorialParts.emplace(text1);
    registry.tutorialParts.emplace(adjustment_text);
    registry.tutorialParts.emplace(curr_timing_text);


    if (!gameInfo.in_options) {
        Entity contText = createText("Press space to continue", vec2(gameInfo.width / 2.f, gameInfo.height * 7.5f / 8.f), 0.9f, Colour::white, Screen::TUTORIAL, true, true);
        registry.tutorialParts.emplace(contText);
    }

    // Create +/- frames buttons

    // vec2 shadow_pos = vec2(centerX - shift, centerY) + vec2(10.f, 10.f);
    vec2 buttonSize = vec2(gameInfo.height / 10.f, gameInfo.height / 10.f);
    //Entity easy_shadow = createBox(shadow_pos, buttonSize);
    //registry.screens.insert(easy_shadow, Screen::TUTORIAL);
    //registry.colours.insert(easy_shadow, Colour::theme_blue_3);
    increase_frames_btn = createButton("+", vec2(centerX + shift, centerY), 1.5f, buttonSize, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::TUTORIAL);
    decrease_frames_btn = createButton("-", vec2(centerX - shift, centerY), 1.5, buttonSize, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::TUTORIAL);
    registry.tutorialParts.emplace(increase_frames_btn);
    registry.tutorialParts.emplace(decrease_frames_btn);

    Entity increase_btn_shadow = createBox(vec2(centerX + shift, centerY) + vec2(10.f, 10.f), buttonSize);
    registry.screens.insert(increase_btn_shadow, Screen::TUTORIAL);
    registry.colours.insert(increase_btn_shadow, Colour::theme_blue_3);
    registry.tutorialParts.emplace(increase_btn_shadow);

    Entity decrease_btn_shadow = createBox(vec2(centerX - shift, centerY) + vec2(10.f, 10.f), buttonSize);
    registry.screens.insert(decrease_btn_shadow, Screen::TUTORIAL);
    registry.colours.insert(decrease_btn_shadow, Colour::theme_blue_3);
    registry.tutorialParts.emplace(decrease_btn_shadow);
    

}

bool Tutorial::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];
    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Game Over scene back
    if (is_visible) {
		registry.screens.insert(curr_screen_entity, Screen::TUTORIAL);
	}
    ScreenState& screen = registry.screenStates.components[0];
    if (isVisible) {
        screen.darken_screen_factor = 0.85;
    } else {
        screen.darken_screen_factor = 0;
    }
    return is_visible;
}

// transition to overworld, set difficulty
void Tutorial::handle_difficulty_click(int difficulty) {
    gameInfo.curr_difficulty = difficulty;
    std::cout << "set difficulty: " << difficulty << std::endl;
    //if (gameInfo.prev_screen == Screen::OPTIONS) {
    //    gameInfo.curr_screen = Screen::OPTIONS;
    //} else {
    //    gameInfo.curr_screen = Screen::OVERWORLD;
    //}
}

void Tutorial::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                tutorial_progress++;
                // if playing tutorial from options, don't play choose difficulty or adjust note timing
                if (gameInfo.in_options && (tutorial_progress + 2) == tutorial_max_progress_parts) {
                    tutorial_progress++;
                    std::cout << "space pressed, prev part in 1 index: " << tutorial_progress << std::endl;
                    init_parts((TutorialPart)tutorial_progress);
                    if (gameInfo.prev_screen == Screen::OPTIONS) {
                        gameInfo.curr_screen = Screen::OPTIONS;
                    }
                    else {
                        gameInfo.curr_screen = Screen::OVERWORLD;
                    }
                    // don't go to adjust timing screen if we set difficulty via difficulty button in settings
                } else if (gameInfo.in_options && gameInfo.in_difficulty) {
                    init_parts((TutorialPart)tutorial_progress);
                    gameInfo.in_difficulty = false;
                    if (gameInfo.prev_screen == Screen::OPTIONS) {
                        gameInfo.curr_screen = Screen::OPTIONS;
                    }
                    else {
                        gameInfo.curr_screen = Screen::OVERWORLD;
                    }
                } else {
                    // if (tutorial_progress != tutorial_max_progress_parts) {
                    std::cout << "space pressed, prev part in 1 index: " << tutorial_progress << std::endl;
                    init_parts((TutorialPart) tutorial_progress);
                    if (tutorial_progress == tutorial_max_progress_parts)
                        if (gameInfo.prev_screen == Screen::OPTIONS) {
                            gameInfo.curr_screen = Screen::OPTIONS;
                        }
                        else {
                            gameInfo.curr_screen = Screen::OVERWORLD;
                        }
                    // }
                }
            }
            break;
        // case GLFW_KEY_X:
        //     if (action == GLFW_PRESS) {
        //         debugging.in_debug_mode = !debugging.in_debug_mode;
        //     }
        //     break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
}

void Tutorial::handle_mouse_move(MouseArea mouse_area) {
    this->mouse_area = mouse_area;
}