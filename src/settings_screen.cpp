#include "settings_screen.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Settings::Settings() {

}

Settings::~Settings() {

};

void Settings::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
};

void Settings::init_screen() {
    createText(":H", vec2(10.f, 35.f), 0.75f, Colour::theme_blue_3, Screen::SETTINGS, false, true);
}

bool Settings::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Settings/Help";

    glfwSetWindowTitle(window, title_ss.str().c_str());

    // Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

    return true;
}

bool Settings::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Settings scene back
    if (is_visible) {
		registry.screens.insert(curr_screen_entity, Screen::SETTINGS);
	}

    return is_visible;
}

// exit settings by pressing ESC or SPACE
void Settings::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) { 
                gameInfo.curr_screen = Screen::OVERWORLD;
                std::cout << "test return to overworld after settings " << std::endl;
            }
            break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
}

void Settings::handle_mouse_move(vec2 pos) {
    
};