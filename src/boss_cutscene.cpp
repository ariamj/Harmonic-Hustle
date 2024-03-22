#include "boss_cutscene.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"


BossCutscene::BossCutscene() {

}

BossCutscene::~BossCutscene() {

};

void BossCutscene::init(GLFWwindow* window, RenderSystem* renderer) {
     is_visible = false;
    this->window = window;
    this->renderer = renderer;
};

bool BossCutscene::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Boss Cutscene";
	title_ss << " --- FPS: " << FPS;

    glfwSetWindowTitle(window, title_ss.str().c_str());

    float xDisplacement = CS_WIDTH * 3.f / 7.f;

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

    if ((dialogue_progress + 1) % 2 == 0) {
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
        Entity text = createText(DIALOGUE[dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::BOSS_CS);
        registry.CSTexts.emplace(text);

        Entity continue_text = createText(CONT_TEXT, vec2(gameInfo.width / 1.4, gameInfo.height / 1.15), 0.5, Colour::black, glm::mat4(1.f), Screen::BOSS_CS);
    }
    else {
        for (auto& e : registry.CSTexts.entities) {
            registry.texts.remove(e);
        }

        Entity text = createText(DIALOGUE[dialogue_progress], vec2(gameInfo.width / 2.f, gameInfo.height / 1.3), 0.5, Colour::black, glm::mat4(1.f), Screen::BOSS_CS);
        registry.CSTexts.emplace(text);
        
    }

    // Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

    return true;
}

bool BossCutscene::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add boss cutscene scene back
    if (is_visible) {
		registry.screens.insert(curr_screen_entity, Screen::BOSS_CS);
	}

    return is_visible;
}

// exit settings by pressing ESC or SPACE
void BossCutscene::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                std::cout << "NEXT DIALOGUE" << std::endl;
                dialogue_progress++;
            }
            break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
}

void BossCutscene::handle_mouse_move(vec2 pos) {
    
};