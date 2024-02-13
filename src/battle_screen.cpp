#include "battle_screen.hpp"
// #include "world_init.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include "world_init.hpp"

Battle::Battle() {
    rng = std::default_random_engine(std::random_device()());
}

Battle::~Battle() {

};

void Battle::init() {
    is_visible = false;
};

bool Battle::handle_step(float elapsed_ms) {

};

bool Battle::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    return is_visible;
};

void Battle::handle_collisions() {

};

void handleRhythmInput(int action, int key) {
	if (action == GLFW_PRESS) {
        std::cout << "rhythm input: " << key << std::endl;
	}
}

void Battle::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_D:
		case GLFW_KEY_F:
		case GLFW_KEY_J:
		case GLFW_KEY_K:
            handleRhythmInput(action, key);
            break;
        default:
            std::cout << "unhandled key" << std::endl;
            break;
    }
};

void Battle::handle_mouse_move(vec2 pos) {
    
};