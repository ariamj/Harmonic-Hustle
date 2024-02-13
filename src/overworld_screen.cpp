#include "overworld_screen.hpp"
// #include "world_init.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include "world_init.hpp"

Overworld::Overworld() {
    rng = std::default_random_engine(std::random_device()());
}

Overworld::~Overworld() {

};

void Overworld::init() {
    is_visible = false;
};

bool Overworld::handle_step(float elapsed_ms) {

};

bool Overworld::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    return is_visible;
};

void Overworld::handle_collisions() {

};

void handleMovementInput(int action, int key) {
	if (action == GLFW_PRESS) {
		std::cout << key << " OVERWORLD PRESSED" << '\n';
	}
	else if (action == GLFW_RELEASE) {
		std::cout << key << " OVERWORLD RELEASED" << '\n';
	}
}

void Overworld::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_W:
		case GLFW_KEY_A:
		case GLFW_KEY_S:
		case GLFW_KEY_D:
            handleMovementInput(action, key);
			break;
        default:
            std::cout << "unhandled overworld key" << std::endl;
            break;
    }
};

void Overworld::handle_mouse_move(vec2 pos) {
    
};