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
    renderButtons();
}

void Settings::renderButtons()
{
    //vec2 shadow_pos = vec2(centerX - shift, centerY) + vec2(10.f, 10.f);
    vec2 buttonPos = vec2(gameInfo.width - 100.f, 100.f);
    vec2 buttonSize = vec2(100.f, 100.f);
    Entity settings_shadow = createBox(buttonPos + vec2(10.f, 10.f), buttonSize);
	registry.screens.insert(settings_shadow, Screen::SETTINGS);
	registry.colours.insert(settings_shadow, Colour::theme_blue_1);
    
    back_btn = createButton("X", buttonPos, 1.5f, buttonSize, Colour::red, Colour::white, Screen::SETTINGS);
}

bool Settings::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Settings/Help";

    glfwSetWindowTitle(window, title_ss.str().c_str());

    // Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

    for (Entity entity : registry.boxButtons.entities) {
        if (registry.screens.get(entity) == Screen::SETTINGS) {
            BoxButton btn = registry.boxButtons.get(entity);
            Motion motion = registry.motions.get(entity);
            std::string text = registry.boxButtons.get(entity).text;
            vec3 text_colour = btn.text_colour;
            if (text == "X" && mouse_area == in_back_btn) {
                text_colour = Colour::dark_red;
            }
            createText(text, motion.position + vec2(5.f,0.f), btn.text_scale, text_colour, Screen::SETTINGS, true, false);
        }
    }

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
    
}
void Settings::handle_mouse_move(MouseArea mouse_area)
{
    this->mouse_area = mouse_area;
}
;