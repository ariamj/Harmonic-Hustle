#include "game_over_screen.hpp"

#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

#include <sstream>
#include <iostream>

GameOver::GameOver() {}

GameOver::~GameOver() {}

void GameOver::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
}

void GameOver::init_screen() {
    title_1_pos = vec2(gameInfo.width/2.f, gameInfo.height/8.f);
    // title "banner"
    Entity banner_1 = createBox(title_1_pos, vec2(950.f, 100.f));
    registry.screens.insert(banner_1, Screen::GAMEOVER);
    registry.colours.insert(banner_1, Colour::white);

    Entity csPlayer = createCSPlayer(renderer, vec2(gameInfo.width/16.f, gameInfo.height/2.f), Screen::GAMEOVER);
    Entity csEnemy = createCSEnemy(renderer, vec2(gameInfo.width*7/8.f, gameInfo.height/2.f), Screen::GAMEOVER);
    Motion& csPlayer_motion = registry.motions.get(csPlayer);
    csPlayer_motion.scale = csPlayer_motion.scale * 1.5f;
    Motion& csEnemy_motion = registry.motions.get(csEnemy);
    csEnemy_motion.scale = csEnemy_motion.scale * 1.5f;

    renderButtons();

    createText("...or press 'space' for Main Menu...", vec2(gameInfo.width/2.f, gameInfo.height*7/8.f), 0.5f, Colour::theme_blue_2, Screen::GAMEOVER, true, true);
}

void GameOver::renderButtons() {
	vec2 main_menu_size = vec2(550.f, gameInfo.height/12.f);
	float y_padding = main_menu_size.y + 15.f;
	vec2 center_pos = vec2(gameInfo.width/2.f, gameInfo.height/2.f);
	vec2 shadow_pos = center_pos + vec2(10.f, 10.f);

	// Main menu button
	Entity start_shadow = createBox(shadow_pos, main_menu_size);
	registry.screens.insert(start_shadow, Screen::GAMEOVER);
	registry.colours.insert(start_shadow, Colour::theme_blue_3);
	restart_btn = createButton("MAIN MENU", center_pos, 1.5f, main_menu_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::GAMEOVER);
}

bool GameOver::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
    title_ss << "Harmonic Hustle";
    glfwSetWindowTitle(window, title_ss.str().c_str());

    // Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());
    

    vec2 shadow_offset = vec2(10.f);
    std::string game_over_title;
    if (gameInfo.won_boss) {
        game_over_title = "VICTORY!!";
    } else {
        game_over_title = "GAME OVER";
    }
    // title - shadow
    createText(game_over_title, title_1_pos + shadow_offset, 2.5f, Colour::theme_blue_3, Screen::GAMEOVER, true);
    // title
    createText(game_over_title, title_1_pos, 2.5f, Colour::theme_blue_1, Screen::GAMEOVER, true);
    
    // Render all button texts
    for (Entity entity : registry.boxButtons.entities) {
        if (registry.screens.get(entity) == Screen::GAMEOVER) {
            BoxButton btn = registry.boxButtons.get(entity);
            Motion motion = registry.motions.get(entity);
            std::string text = registry.boxButtons.get(entity).text;
            vec3 text_colour = btn.text_colour;

            // Hover effect
            // NOTE: if lag happens, comment this part out
            if ((text == "MAIN MENU" && mouse_area == in_restart_btn)) {
                text_colour = Colour::white;
            }
            createText(text, motion.position, btn.text_scale, text_colour, Screen::GAMEOVER, true);
        }
    }

    // Debug
    if (debugging.in_debug_mode) {
        for (Entity entity : registry.boxButtons.entities) {
            Motion motion = registry.motions.get(entity);
            vec2 pos = motion.position;
            BoxAreaBound bound = registry.boxAreaBounds.get(entity);
            createLine(vec2(bound.left, pos.y), vec2(5, motion.scale.y), Screen::GAMEOVER);
            createLine(vec2(bound.right, pos.y), vec2(5, motion.scale.y), Screen::GAMEOVER);
            createLine(vec2(pos.x, bound.top), vec2(motion.scale.x, 5), Screen::GAMEOVER);
            createLine(vec2(pos.x, bound.bottom), vec2(motion.scale.x, 5), Screen::GAMEOVER);
        }
    }
    
    return true;
}

bool GameOver::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Game Over scene back
    if (is_visible) {
		registry.screens.insert(curr_screen_entity, Screen::GAMEOVER);
	}
    
    // ScreenState& screen = registry.screenStates.components[0];
    // if (isVisible) {
    //     screen.darken_screen_factor = 0.2;
    // } else {
    //     screen.darken_screen_factor = 0;
    // }

    return is_visible;
}

// Input callback functions
void GameOver::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                std::cout << "space pressed" << std::endl;
                gameInfo.curr_screen = Screen::START;
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

void GameOver::handle_mouse_move(MouseArea mouse_area) {
    this->mouse_area = mouse_area;
}
