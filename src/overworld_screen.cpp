#include "overworld_screen.hpp"
#include "serializer.hpp"
#include <cassert>
#include <sstream>
#include <iostream>
#include <math.h>
#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

// Game configuration
// const size_t MAX_ENEMIES = 2;
// const size_t ENEMY_DELAY_MS = 5000 * 3;
const float PLAYER_SPEED = 200.f;

Serializer saver = Serializer();

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
	title_ss << " --- FPS: " << FPS;
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

	Motion& playerMotion = registry.motions.get(*gameInfo.player_sprite);

	std::string levelText = "lvl " + std::to_string(gameInfo.curr_level);
	createText(levelText, vec2(playerMotion.position[0], playerMotion.position[1] - PLAYER_HEIGHT / 2.f - 10.f), 0.5f, Colour::green, glm::mat4(1.f), Screen::OVERWORLD, true );
	

	// // Spawn new enemies
	// next_enemy_spawn -= elapsed_ms_since_last_update * current_speed;
	// if (registry.enemies.components.size() <= MAX_ENEMIES && next_enemy_spawn < 0.f) {
	// 	// reset timer
	// 	next_enemy_spawn = (ENEMY_DELAY_MS / 2) + uniform_dist(rng) * (ENEMY_DELAY_MS / 2);
	// 	// create an enemy

	// 	createEnemy(renderer, vec2(50.f + uniform_dist(rng) * (window_width_px - 100.f), window_height_px / 3), 1);
	// }

    // check if enemies are high level, if yes color red, else remove color if needed
    auto& enemies = registry.enemies.entities;
    int playerLevel = registry.levels.get(*gameInfo.player_sprite).level;
    for (Entity enemy : enemies) {
        int enemyLevel = registry.levels.get(enemy).level;
		Motion& enemyMotion = registry.motions.get(enemy);
		std::string levelText = "lvl " + std::to_string(enemyLevel);
        if (enemyLevel > playerLevel) {
            if (!registry.colours.has(enemy)) {
                registry.colours.insert(enemy, {0.95f, 0.6f, 0.6f});
            }
			createText(levelText, vec2(enemyMotion.position[0], enemyMotion.position[1] - ENEMY_HEIGHT / 2.f - 10.f), 0.5f, Colour::red, glm::mat4(1.f), Screen::OVERWORLD, true);
        } else {
            if (registry.colours.has(enemy)) {
                registry.colours.remove(enemy);
            }
			createText(levelText, vec2(enemyMotion.position[0], enemyMotion.position[1] - ENEMY_HEIGHT / 2.f - 10.f), 0.5f, Colour::white, glm::mat4(1.f), Screen::OVERWORLD, true);
        }
    }

    // count down enemy pause timer if needed
    if (registry.pauseEnemyTimers.has(*gameInfo.player_sprite)) {
        PauseEnemyTimer& timer = registry.pauseEnemyTimers.get(*gameInfo.player_sprite);
        timer.counter_ms = timer.counter_ms - elapsed_ms_since_last_update;
        if (timer.counter_ms <= 0.f) {
            registry.pauseEnemyTimers.remove(*gameInfo.player_sprite);
        }
    }

	// Process the player state
	// assert(registry.screenStates.components.size() <= 1);
	// ScreenState &screen = registry.screenStates.components[0];

	// float min_counter_ms = 3000.f;

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

// return true if game should restart
//  on collisions for now -> remove ALL enemies with the same level (TODO -> update if needed)
bool Overworld::handle_collisions() {
    auto& collisionsRegistry = registry.collisions;
    for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
        Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

        // if collision between player and enemy, switch to battle scene, remove enemy
        if (registry.players.has(entity) && registry.enemies.has(entity_other)) {
            // int enemyLevel = registry.levels.get(entity_other).level;

            // if collision is between enemy with level <= player level
            //      set curr enemy and switch to battle scene
            // else restart the entire game
			if (registry.isRunning.has(entity_other)) {

				// Set enemy sprite as enemy for battle
				gameInfo.curr_enemy = entity_other;
				gameInfo.curr_screen = Screen::BATTLE;

            } else {
				// Set enemy sprite as enemy for battle - TEMP, TODO: use one is above if when implemented
				gameInfo.curr_enemy = entity_other;
				gameInfo.curr_level = 1;
				// return true to restart game
				return true;
            }
        }
    }
    return false;
};

void handleMovementInput(int action, int key) {
	Entity e = registry.players.entities[0];
	Motion& motion = registry.motions.get(e);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) {
			motion.velocity[1] = -PLAYER_SPEED;
		}
		if (key == GLFW_KEY_S) {
			motion.velocity[1] = PLAYER_SPEED;
		}
		if (key == GLFW_KEY_A) {
			motion.velocity[0] = -PLAYER_SPEED;
		}
		if (key == GLFW_KEY_D) {
			motion.velocity[0] = PLAYER_SPEED;
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

void handleDebugInput(int action) {
	if (action == GLFW_PRESS) {
		debugging.in_debug_mode = !debugging.in_debug_mode;
	}
}

void Overworld::handle_key(int key, int scancode, int action, int mod) {
	if (key == GLFW_KEY_S && action == GLFW_PRESS && (mod & GLFW_MOD_CONTROL)) {
		printf("Ctrl+S detected, game saving");
		saver.save_game();
	}
	else {
		switch(key) {
		case GLFW_KEY_W:
		case GLFW_KEY_A:
		case GLFW_KEY_S:
		case GLFW_KEY_D:
			handleMovementInput(action, key);
			break;
		case GLFW_KEY_X:
			// toggle debug
			handleDebugInput(action);
		default:
			std::cout << "unhandled overworld key" << std::endl;
			break;
		}
	}
    
};

void Overworld::handle_mouse_move(vec2 pos) {
    
};