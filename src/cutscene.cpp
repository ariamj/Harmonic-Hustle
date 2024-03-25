#include "cutscene.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"


Cutscene::Cutscene() {

}

Cutscene::~Cutscene() {

};

void Cutscene::init(GLFWwindow* window, RenderSystem* renderer) {
     is_visible = false;
    this->window = window;
    this->renderer = renderer;
};

bool Cutscene::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Cutscene";
	title_ss << " --- FPS: " << FPS;

    glfwSetWindowTitle(window, title_ss.str().c_str());

    float xDisplacement = CS_WIDTH * 3.f / 7.f;

    if (!is_intro_finished) {
        if (registry.enemyCS.components.size() == 0) {
            Entity enemy = createCSEnemy(this->renderer, vec2(gameInfo.width - xDisplacement - 40.f, gameInfo.height / 2.f));
            registry.enemyCS.emplace(enemy);
        }

        if (registry.playerCS.components.size() == 0) {
            Entity player = createCSPlayer(this->renderer, vec2(xDisplacement, gameInfo.height / 2.f));
            registry.playerCS.emplace(player);
        }

        if (registry.CSTextbox.components.size() == 0) {
            Entity textbox = createCSTextbox(this->renderer, vec2(gameInfo.width / 2.f, gameInfo.height / 1.3));
            registry.CSTextbox.emplace(textbox);
        }

        if ((intro_dialogue_progress + 1) % 2 == 0) {
            Entity e = registry.playerCS.entities[0];
            Entity enemy = registry.enemyCS.entities[0];

            if (!registry.colours.has(e)) {
                registry.colours.insert(e, Colour::dark_gray);
            }

            if (registry.colours.has(enemy)) {
                registry.colours.remove(enemy);
            }
        }
        else {
            Entity e = registry.enemyCS.entities[0];
            Entity player = registry.playerCS.entities[0];

            if (!registry.colours.has(e)) {
                registry.colours.insert(e, Colour::dark_gray);
            }

            if (registry.colours.has(player)) {
                registry.colours.remove(player);
            }

        }

        if (registry.CSTexts.components.size() == 0) {
            Entity text = createText(INTRO_DIALOGUE[intro_dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
            registry.CSTexts.emplace(text);

            // continue text
            createText(CONT_TEXT, vec2(gameInfo.width / 1.4, gameInfo.height / 1.15), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
        }
        else {
            for (auto& e : registry.CSTexts.entities) {
                registry.texts.remove(e);
            }

            Entity text = createText(INTRO_DIALOGUE[intro_dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
            registry.CSTexts.emplace(text);
        }
    }
    else if (!is_boss_finished) {
        if (registry.enemyCS.components.size() == 0) {
            Entity enemy = createCSEnemy(this->renderer, vec2(gameInfo.width - xDisplacement - 40.f, gameInfo.height / 2.f));
            registry.enemyCS.emplace(enemy);
        }

        if (registry.playerCS.components.size() == 0) {
            Entity player = createCSPlayer(this->renderer, vec2(xDisplacement, gameInfo.height / 2.f));
            registry.playerCS.emplace(player);
        }

        if (registry.CSTextbox.components.size() == 0) {
            Entity textbox = createCSTextbox(this->renderer, vec2(gameInfo.width / 2.f, gameInfo.height / 1.3));
            registry.CSTextbox.emplace(textbox);
        }

        if ((boss_dialogue_progress + 1) % 2 == 0) {
            Entity e = registry.playerCS.entities[0];
            Entity enemy = registry.enemyCS.entities[0];

            if (!registry.colours.has(e)) {
                registry.colours.insert(e, Colour::dark_gray);
            }

            if (registry.colours.has(enemy)) {
                registry.colours.remove(enemy);
            }
        }
        else {
            Entity e = registry.enemyCS.entities[0];
            Entity player = registry.playerCS.entities[0];

            if (!registry.colours.has(e)) {
                registry.colours.insert(e, Colour::dark_gray);
            }

            if (registry.colours.has(player)) {
                registry.colours.remove(player);
            }
        }

        if (registry.CSTexts.components.size() == 0) {
            Entity text = createText(BOSS_DIALOGUE[boss_dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
            registry.CSTexts.emplace(text);

            // continue text
            createText(CONT_TEXT, vec2(gameInfo.width / 1.4, gameInfo.height / 1.15), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
        }
        else {
            for (auto& e : registry.CSTexts.entities) {
                registry.texts.remove(e);
            }

            Entity text = createText(BOSS_DIALOGUE[boss_dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
            registry.CSTexts.emplace(text);
        }
    }else if (!is_game_over_finished) {
         if (registry.enemyCS.components.size() == 0) {
            Entity enemy = createCSEnemy(this->renderer, vec2(gameInfo.width - xDisplacement - 40.f, gameInfo.height / 2.f));
            registry.enemyCS.emplace(enemy);
        }

        if (registry.playerCS.components.size() == 0) {
            Entity player = createCSPlayer(this->renderer, vec2(xDisplacement, gameInfo.height / 2.f));
            registry.playerCS.emplace(player);
        }

        if (registry.CSTextbox.components.size() == 0) {
            Entity textbox = createCSTextbox(this->renderer, vec2(gameInfo.width / 2.f, gameInfo.height / 1.3));
            registry.CSTextbox.emplace(textbox);
        }

        if ((game_over_dialogue_progress + 1) % 2 == 0) {
            Entity e = registry.playerCS.entities[0];
            Entity enemy = registry.enemyCS.entities[0];

            if (!registry.colours.has(e)) {
                registry.colours.insert(e, Colour::dark_gray);
            }

            if (registry.colours.has(enemy)) {
                registry.colours.remove(enemy);
            }
        }
        else {
            Entity e = registry.enemyCS.entities[0];
            Entity player = registry.playerCS.entities[0];

            if (!registry.colours.has(e)) {
                registry.colours.insert(e, Colour::dark_gray);
            }

            if (registry.colours.has(player)) {
                registry.colours.remove(player);
            }

        }

        if (registry.CSTexts.components.size() == 0) {
            Entity text = createText(GAME_OVER_DIALOGUE[game_over_dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
            registry.CSTexts.emplace(text);

            // continue text
            createText(CONT_TEXT, vec2(gameInfo.width / 1.4, gameInfo.height / 1.15), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
        }
        else {
            for (auto& e : registry.CSTexts.entities) {
                registry.texts.remove(e);
            }

            Entity text = createText(GAME_OVER_DIALOGUE[game_over_dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::CUTSCENE);
            registry.CSTexts.emplace(text);
        }
    }

    // Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

    return true;
}

void Cutscene::remove_prev_assets() {
    registry.CSTextbox.clear();
    registry.enemyCS.clear();
    registry.playerCS.clear();
}

bool Cutscene::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add boss cutscene scene back
    if (is_visible) {
		registry.screens.insert(curr_screen_entity, Screen::CUTSCENE);
	}

    return is_visible;
}

// exit settings by pressing ESC or SPACE
void Cutscene::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                std::cout << "NEXT DIALOGUE" << std::endl;
                if (!is_intro_finished) {
                    intro_dialogue_progress++;
                }
                else if (!is_boss_finished) {
                    boss_dialogue_progress++;
                } else {
                    game_over_dialogue_progress++;
                }
            }
            break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
}

void Cutscene::handle_mouse_move(vec2 pos) {
    
};