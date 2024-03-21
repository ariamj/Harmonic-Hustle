#include "start_screen.hpp"

#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

#include <sstream>
#include <iostream>

Start::Start() {

}

Start::~Start() {

}

void Start::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;

}

bool Start::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
    title_ss << "Harmonic Hustle --- Settings/Help";
    glfwSetWindowTitle(window, title_ss.str().c_str());

    // Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());
    
    createText("START", vec2(gameInfo.width/2.f, gameInfo.height/2.f), 2.f, Colour::purple, glm::mat4(1.f), Screen::START, true);

    return true;
}

bool Start::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Start scene back
    if (is_visible) {
		registry.screens.insert(curr_screen_entity, Screen::START);
	}

    return is_visible;
}

// Input callback functions
void Start::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                gameInfo.curr_screen = Screen::OVERWORLD;
            }
            break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
}

void Start::handle_mouse_move(vec2 pos) {

}