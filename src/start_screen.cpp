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
    
    for (Entity entity : registry.boxButtons.entities) {
        if (registry.screens.get(entity) == Screen::START) {
            BoxButton btn = registry.boxButtons.get(entity);
            Motion motion = registry.motions.get(entity);
            std::string text = registry.boxButtons.get(entity).text;
            createText(text, motion.position, btn.text_scale, btn.text_colour, glm::mat4(1.f), Screen::START, true);
        }
    }

    // Debug
    if (debugging.in_debug_mode) {
        for (Entity entity : registry.boxButtons.entities) {
            Motion motion = registry.motions.get(entity);
            vec2 pos = motion.position;
            BoxAreaBound bound = registry.boxAreaBounds.get(entity);
            createLine(vec2(bound.left, pos.y), vec2(5, motion.scale.y), Screen::START);
            createLine(vec2(bound.right, pos.y), vec2(5, motion.scale.y), Screen::START);
            createLine(vec2(pos.x, bound.top), vec2(motion.scale.x, 5), Screen::START);
            createLine(vec2(pos.x, bound.bottom), vec2(motion.scale.x, 5), Screen::START);
        }
    }
    

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
                std::cout << "space pressed" << std::endl;
                gameInfo.curr_screen = Screen::OVERWORLD;
            }
            break;
        case GLFW_KEY_X:
            if (action == GLFW_PRESS) {
                debugging.in_debug_mode = !debugging.in_debug_mode;
            }
            break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
}

void Start::handle_mouse_move(vec2 pos) {
    (vec2)pos; // silence warning
}