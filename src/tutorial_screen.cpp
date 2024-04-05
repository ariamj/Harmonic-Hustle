#include "tutorial_screen.hpp"

#include <sstream>
#include <iostream>
#include "world_init.hpp"

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
    createText("Tutorial", vec2(gameInfo.width / 2.f, gameInfo.height / 20.f), 1.3f, Colour::white, Screen::TUTORIAL );

    if (tutorial_progress == TutorialPart::INTRO) {
        renderIntroText();

    } else if (tutorial_progress == TutorialPart::BATTLE_EXPLAIN) {
        renderBattleExplainText();

    } else if (tutorial_progress == TutorialPart::ADVANCING_EXPLAIN) {
        renderAdvancingExplainText();
    }

    createText("Press space to continue", vec2(gameInfo.width/2.f, gameInfo.height * 7.5f / 8.f), 0.9f, Colour::white, Screen::TUTORIAL);
    return true;
}


/////////////RENDER TEXT HELPER METHODS


void Tutorial::renderIntroText() {
    vec2 spriteExPos = {gameInfo.width * 8.5f / 10.f, gameInfo.height * 1.5f / 6.f - PLAYER_HEIGHT / 2.f - 10.f};
    createText("lvl 1", spriteExPos, 0.5f, Colour::green, Screen::TUTORIAL );

    vec2 enemyExPostion = vec2(gameInfo.width * 1.5f / 10.f, gameInfo.height * 3.f / 6.f - PLAYER_HEIGHT / 2.f - 10.f);
    createText("lvl 1", enemyExPostion, 0.5f, Colour::white, Screen::TUTORIAL );

    vec2 redEnemyExPosition = vec2(gameInfo.width * 8.5f / 10.f, gameInfo.height * 4.5f / 6.f - PLAYER_HEIGHT / 2.f - 10.f);
    createText("lvl 2", redEnemyExPosition, 0.5f, Colour::red, Screen::TUTORIAL );

    float currXPos = gameInfo.width * 4.f / 10.f;
    float currYPos = gameInfo.height * 1.3f/ 7.f;

    createText("This is you! You're currently level 1. To win this game,",
        vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL);

    currYPos += 30.f;
    createText("you'll have to level up to be strong enough to defeat the boss.",
        vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL);

    currYPos += 50.f;
    createText("On spawn, you'll be in the overworld surrounded by enemies.",
        vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL);

    currYPos += 40.f;
    Entity wasd = createText("W A S D",
        vec2(gameInfo.width * 2.5f / 10.f, currYPos), 0.65f, Colour::green, Screen::TUTORIAL);

    Entity wasdRest = createText("keys to move around",
        vec2(gameInfo.width * 4.5f / 10.f, currYPos), 0.65f, Colour::off_white, Screen::TUTORIAL);

    ///////////// right aligned
    float rightXPos = gameInfo.width * 6.f / 10.f;
    currYPos += 140.f;
    createText("On spawn, you'll be in the overworld surrounded by enemies.", 
        vec2(rightXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL);

    currYPos += 40.f;
    createText("Collide and ", 
        vec2(rightXPos - 270.f, currYPos), 0.65f, Colour::off_white, Screen::TUTORIAL);
    createText("defeat same level enemies", 
        vec2(rightXPos + 150.f, currYPos), 0.65f, Colour::red, Screen::TUTORIAL);

    currYPos += 40.f;
    createText("They'll have a white level tag", 
        vec2(rightXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL);

    ////////left aligned
    currYPos += 170.f;
    createText("run away", 
        vec2(currXPos - 190.f, currYPos), 0.65f, Colour::red, Screen::TUTORIAL);
    createText("from red enemies!", 
        vec2(currXPos + 100.f, currYPos), 0.65f, Colour::off_white, Screen::TUTORIAL);

    currYPos += 40.f;
    createText("they have a higher level than you.", 
        vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL);
    currYPos += 30.f;
    // TODO -> update if we rework the red enemy collision flow
    createText("if you hit them, you'll have to restart the game!", 
        vec2(currXPos, currYPos), 0.5f, Colour::off_white, Screen::TUTORIAL);
}


void Tutorial::renderBattleExplainText() {
    // createText("TESTING 2", vec2(gameInfo.width/2.f, gameInfo.height / 2.f), 2.f, Colour::white, Screen::TUTORIAL, true);
    vec2 spriteExPos = {gameInfo.width * 1.5f / 10.f, gameInfo.height * 0.85f / 6.f - PLAYER_HEIGHT * 0.7f / 2.f - 7.f };
    createText("lvl 1", spriteExPos, 0.5f * 0.7f, Colour::white, Screen::TUTORIAL );

    float rightXPos = gameInfo.width * 5.f / 10.f;
    float currY = gameInfo.height / 7.f;
    createText("If you hit one of these guys, you'll enter a battle!",
        vec2(rightXPos, currY), 0.5f, Colour::off_white, Screen::TUTORIAL, true);

}


void Tutorial::renderAdvancingExplainText() {
    createText("TESTING 3", vec2(gameInfo.width/2.f, gameInfo.height / 2.f), 2.f, Colour::white, Screen::TUTORIAL, true);

}

/////////////INIT HELPER METHODS

void Tutorial::initIntroParts() {
    // vec2 spriteExPos = {gameInfo.width / 10.f, gameInfo.height / 5.f};
    // Entity testing = createHelpImage(renderer, spriteExPos, vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::PLAYER_WALK_F1, Screen::TUTORIAL);
    
    vec2 spriteExPos = {gameInfo.width * 8.5f / 10.f, gameInfo.height * 1.5f / 6.f};
    Entity playerSpriteExample = createHelpImage(renderer, spriteExPos, vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::PLAYER_WALK_F1, Screen::TUTORIAL);
    
    registry.tutorialParts.emplace(playerSpriteExample);

    Entity currEnemyExample = createHelpImage(renderer, vec2(gameInfo.width * 1.5f / 10.f, gameInfo.height * 3.f / 6.f),
            vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::ENEMY_GUITAR, Screen::TUTORIAL);

    registry.tutorialParts.emplace(currEnemyExample);

    Entity redEnemyExample = createHelpImage(renderer, vec2(gameInfo.width * 8.5f / 10.f, gameInfo.height * 4.5f / 6.f),
            vec2(PLAYER_WIDTH, PLAYER_HEIGHT), TEXTURE_ASSET_ID::ENEMY_MIC, Screen::TUTORIAL);

    registry.colours.insert(redEnemyExample, Colour::red_enemy_tint);

    registry.tutorialParts.emplace(redEnemyExample);
}

void Tutorial::initBattleExplainParts() {
    vec2 spriteExPos = {gameInfo.width * 1.5f / 10.f, gameInfo.height * 0.85f / 6.f};
    Entity playerSpriteExample = createHelpImage(renderer, spriteExPos, vec2(PLAYER_WIDTH, PLAYER_HEIGHT) * vec2(0.7), TEXTURE_ASSET_ID::ENEMY_GUITAR, Screen::TUTORIAL);
    
    registry.tutorialParts.emplace(playerSpriteExample);
}

void Tutorial::initAdvancingExplaingParts() {

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

void Tutorial::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                tutorial_progress++;
                std::cout << "space pressed, prev part in 1 index: " << tutorial_progress << std::endl;
                init_parts((TutorialPart) tutorial_progress);
                if (tutorial_progress == tutorial_max_progress_parts)
                    gameInfo.curr_screen = Screen::OVERWORLD;
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