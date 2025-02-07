#include "start_screen.hpp"

#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

#include <sstream>
#include <iostream>

Start::Start() {}

Start::~Start() {}

void Start::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
}

void Start::init_screen() {
    title_1_pos = vec2(gameInfo.width/2.f - gameInfo.width/8.f, gameInfo.height/8.f);
    title_2_pos = vec2(gameInfo.width/2.f + gameInfo.width/8.f, gameInfo.height*2/8.f);
    // title "banner"
    Entity banner_1 = createBox(title_1_pos, vec2(800.f, 100.f));
    registry.screens.insert(banner_1, Screen::START);
    registry.colours.insert(banner_1, Colour::white);
    Entity banner_2 = createBox(title_2_pos, vec2(650.f, 100.f));
    registry.screens.insert(banner_2, Screen::START);
    registry.colours.insert(banner_2, Colour::white);

    vec2 shadow_offset = vec2(10.f);
    // title - shadow
    createText("HARMONIC", title_1_pos + shadow_offset, 2.5f, Colour::theme_blue_3, Screen::START, true, true);
    createText("HUSTLE", title_2_pos + shadow_offset, 2.5f, Colour::theme_blue_3, Screen::START, true, true);
    // title
    createText("HARMONIC", title_1_pos, 2.5f, Colour::theme_blue_1, Screen::START, true, true);
    createText("HUSTLE", title_2_pos, 2.5f, Colour::theme_blue_1, Screen::START, true, true);

    renderButtons();

    createText("...or press 'space' for new game...", vec2(gameInfo.width/2.f, gameInfo.height*7/8.f), 0.5f, Colour::theme_blue_2, Screen::START, true, true);
}

void Start::renderButtons() {
	// vec2 main_menu_size = vec2(350.f, gameInfo.height/12.f);
    vec2 main_menu_size = vec2(500.f, gameInfo.height/12.f);
    // vec2 new_game_btn_size = vec2(525.f, gameInfo.height/9.5f);
    vec2 new_game_btn_size = vec2(570.f, gameInfo.height/8.7f);
    float new_game_y_padding = new_game_btn_size.y + 15.f;
	float y_padding = main_menu_size.y + 15.f;
	vec2 center_pos = vec2(gameInfo.width/2.f, gameInfo.height/2.f);
	vec2 shadow_pos = center_pos + vec2(10.f, 10.f);

	// Start button
	Entity start_shadow = createBox(shadow_pos, new_game_btn_size);
	registry.screens.insert(start_shadow, Screen::START);
	registry.colours.insert(start_shadow, Colour::theme_blue_3);
	start_btn = createButton("NEW GAME", center_pos, 1.5f, new_game_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::START);
	
     // load from save button
    Entity save_shadow = createBox(vec2(0, new_game_y_padding) + shadow_pos, main_menu_size);
	registry.screens.insert(save_shadow, Screen::START);
	registry.colours.insert(save_shadow, Colour::theme_blue_3);
	load_from_save_btn = createButton("LOAD", center_pos + vec2(0, new_game_y_padding), 1.5f, main_menu_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::START);

	// Settings button
	Entity settings_shadow = createBox(vec2(0, y_padding + y_padding+ 40.f) + shadow_pos, main_menu_size);
	registry.screens.insert(settings_shadow, Screen::START);
	registry.colours.insert(settings_shadow, Colour::theme_blue_3);
	settings_btn = createButton("SETTINGS", center_pos + vec2(0, y_padding + y_padding + 40.f), 1.5f, main_menu_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::START);
}

bool Start::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
    title_ss << "Harmonic Hustle";
    glfwSetWindowTitle(window, title_ss.str().c_str());

    // Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());
    
    // Render all button texts
    for (Entity entity : registry.boxButtons.entities) {
        if (registry.screens.get(entity) == Screen::START) {
            BoxButton btn = registry.boxButtons.get(entity);
            Motion motion = registry.motions.get(entity);
            std::string text = registry.boxButtons.get(entity).text;
            vec3 text_colour = btn.text_colour;

            // Hover effect
            // NOTE: if lag happens, comment this part out
            if ((text == "NEW GAME" && mouse_area == in_start_btn) || (text == "SETTINGS" && mouse_area == in_settings_btn) || (text == "LOAD" && mouse_area == in_load_btn)) {
                text_colour = Colour::white;
            }
            createText(text, motion.position, btn.text_scale, text_colour, Screen::START, true, false);
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
    
    ScreenState& screen = registry.screenStates.components[0];
    if (isVisible) {
        screen.darken_screen_factor = 0.2;
    } else {
        screen.darken_screen_factor = 0;
    }

    return is_visible;
}

// Input callback functions
void Start::handle_key(int key, int scancode, int action, int mod) {
    switch(key) {
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                std::cout << "space pressed" << std::endl;
                // gameInfo.curr_screen = Screen::OVERWORLD;
                gameInfo.curr_screen = Screen::TUTORIAL;
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

void Start::handle_mouse_move(MouseArea mouse_area) {
    this->mouse_area = mouse_area;
}
