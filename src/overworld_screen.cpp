#include "overworld_screen.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// Game configuration
const size_t MAX_ENEMIES = 2;
const size_t ENEMY_DELAY_MS = 5000 * 3;

float walk_cycle = 0.f;
const float walk_cycle_speed = 0.3;

Overworld::Overworld() 
: next_enemy_spawn(0.f) 
{
    rng = std::default_random_engine(std::random_device()());
}

Overworld::~Overworld() {

};

void Overworld::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
};

bool Overworld::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
	title_ss << "Harmonic Hustle --- Overworld";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Remove out of screen entities (Notes, etc.)
	auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			if(!registry.players.has(motions_registry.entities[i])) // don't remove the player
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// Spawn new enemies
	next_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
	if (registry.enemies.components.size() <= MAX_ENEMIES && next_enemy_spawn < 0.f) {
		// reset timer
		next_enemy_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_DELAY_MS / 2);
		// create an enemy

		createEnemy(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), window_height_px / 3));
	}

	// Process the player state
	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	float min_counter_ms = 3000.f;

    return true;
};

bool Overworld::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    
    Entity& curr_screen_entity = registry.screenStates.entities[0];
    
    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Battle scene back
    if (is_visible) registry.screens.insert(curr_screen_entity, Screen::OVERWORLD);

    return is_visible;
};

// return true if should switch screens -> TODO update to something better if needed...
bool Overworld::handle_collisions() {
    auto& collisionsRegistry = registry.collisions;
    for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
        Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

        // if colision between player and enemy, switch to battle scene, remove enemy
        if (registry.players.has(entity) && registry.enemies.has(entity_other)) {
            registry.enemies.remove(entity_other);
            registry.renderRequests.remove(entity_other);

            return true;
        }
    }
    return false;
};

void handleWalkAnimation() {
	Entity e = registry.players.entities[0];
	RenderRequest& r = registry.renderRequests.get(e);

	walk_cycle += walk_cycle_speed;

	if (walk_cycle <= 1.f) {
		r.used_texture = TEXTURE_ASSET_ID::PLAYER_WALK_F1;
	}
	else if (walk_cycle > 1.f && walk_cycle <= 2.f) {
		r.used_texture = TEXTURE_ASSET_ID::PLAYER_WALK_F2;
	}
	else if (walk_cycle > 2.f && walk_cycle <= 3.f) {
		r.used_texture = TEXTURE_ASSET_ID::PLAYER_WALK_F3;
	}
	else {
		walk_cycle = 0.f;
	}
}

void handleMovementInput(int action, int key) {
	Entity e = registry.players.entities[0];
	Motion& motion = registry.motions.get(e);

	if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
		handleWalkAnimation();

		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_W) {
				motion.velocity[1] = -100;
			}
			if (key == GLFW_KEY_S) {
				motion.velocity[1] = 100;
			}
			if (key == GLFW_KEY_A) {
				motion.velocity[0] = -100;
			}
			if (key == GLFW_KEY_D) {
				motion.velocity[0] = 100;
			}
		}
		else if (action == GLFW_RELEASE) {
			if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
				motion.velocity[1] = 0;
			}
			if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
				motion.velocity[0] = 0;
			}
		}
	}
}

void handleDebug(int action) {
	if (action == GLFW_PRESS) {
		debugging.in_debug_mode = !debugging.in_debug_mode;
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
		case GLFW_KEY_X:
			// toggle debug
			handleDebug(action);
        default:
            std::cout << "unhandled overworld key" << std::endl;
            break;
    }
};

void Overworld::handle_mouse_move(vec2 pos) {
    
};