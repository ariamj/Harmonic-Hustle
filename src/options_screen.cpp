#include "options_screen.hpp"

#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

#include <sstream>
#include <iostream>

OptionsMenu::OptionsMenu() {}

OptionsMenu::~OptionsMenu() {}

void OptionsMenu::init(GLFWwindow* window, RenderSystem* renderer) {
    is_visible = false;
    this->window = window;
    this->renderer = renderer;
}

void OptionsMenu::init_screen() {
    title_1_pos = vec2(gameInfo.width / 2.f, gameInfo.height / 8.f);
    Entity banner_1 = createBox(title_1_pos, vec2(1200.f, 100.f));
    registry.screens.insert(banner_1, Screen::OPTIONS);
    registry.colours.insert(banner_1, Colour::white);

    vec2 shadow_offset = vec2(10.f);
    // title - shadow
    createText("OPTIONS MENU", title_1_pos + shadow_offset, 2.5f, Colour::theme_blue_3, Screen::OPTIONS, true, true);
    // title
    createText("OPTIONS MENU", title_1_pos, 2.5f, Colour::theme_blue_1, Screen::OPTIONS, true, true);

    renderButtons();
}

void OptionsMenu::renderButtons() {
    //vec2 main_menu_size = vec2(350.f, gameInfo.height / 12.f);
    //vec2 new_game_btn_size = vec2(525.f, gameInfo.height / 9.5f);
    vec2 options_btn_size = vec2(600.f, gameInfo.height / 9.5f);
    float new_game_y_padding = options_btn_size.y + 30.f;
    float y_padding = options_btn_size.y + 30.f;
    vec2 center_pos = vec2(gameInfo.width / 2.f, gameInfo.height / 3.f);
    vec2 shadow_pos = center_pos + vec2(10.f, 10.f);
    vec2 shift = vec2(400.f, 0.f);

    // resume game button
    Entity resume_shadow = createBox(shadow_pos - shift, options_btn_size);
    registry.screens.insert(resume_shadow, Screen::OPTIONS);
    registry.colours.insert(resume_shadow, Colour::theme_blue_3);
    resume_game_btn = createButton("RESUME", center_pos - shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

    // new game button
    Entity start_shadow = createBox(vec2(0, new_game_y_padding) + shadow_pos - shift, options_btn_size);
    registry.screens.insert(start_shadow, Screen::OPTIONS);
    registry.colours.insert(start_shadow, Colour::theme_blue_3);
    new_game_btn = createButton("NEW GAME", center_pos + vec2(0, new_game_y_padding) - shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

    // save game button
    Entity save_shadow = createBox(vec2(0, y_padding + y_padding) + shadow_pos - shift, options_btn_size);
    registry.screens.insert(save_shadow, Screen::OPTIONS);
    registry.colours.insert(save_shadow, Colour::theme_blue_3);
    save_game_btn = createButton("SAVE GAME", center_pos + vec2(0, y_padding + y_padding) - shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

    // difficulty settings buttons
    Entity difficulty_shadow = createBox(vec2(0, y_padding + y_padding + y_padding) + shadow_pos - shift, options_btn_size);
    registry.screens.insert(difficulty_shadow, Screen::OPTIONS);
    registry.colours.insert(difficulty_shadow, Colour::theme_blue_3);
    difficulty_btn = createButton("DIFFICULTY", center_pos + vec2(0, y_padding + y_padding + y_padding) - shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

    // controls png button
    Entity controls_shadow = createBox(shadow_pos + shift, options_btn_size);
    registry.screens.insert(controls_shadow, Screen::OPTIONS);
    registry.colours.insert(controls_shadow, Colour::theme_blue_3);
    controls_btn = createButton("CONTROLS", center_pos + shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

    // tutorial button
    Entity tutorial_shadow = createBox(vec2(0, new_game_y_padding) + shadow_pos + shift, options_btn_size);
    registry.screens.insert(tutorial_shadow, Screen::OPTIONS);
    registry.colours.insert(tutorial_shadow, Colour::theme_blue_3);
    tutorial_btn = createButton("TUTORIAL", center_pos + vec2(0, new_game_y_padding) + shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

    // Return to main menu button
    Entity menu_shadow = createBox(vec2(0, y_padding + y_padding) + shadow_pos + shift, options_btn_size);
    registry.screens.insert(menu_shadow, Screen::OPTIONS);
    registry.colours.insert(menu_shadow, Colour::theme_blue_3);
    return_to_main_btn = createButton("MAIN MENU", center_pos + vec2(0, y_padding + y_padding) + shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

    // Save and exit button
    Entity exit_shadow = createBox(vec2(0, y_padding + y_padding + y_padding) + shadow_pos + shift, options_btn_size);
    registry.screens.insert(exit_shadow, Screen::OPTIONS);
    registry.colours.insert(exit_shadow, Colour::theme_blue_3);
    exit_btn = createButton("SAVE + EXIT", center_pos + vec2(0, y_padding + y_padding + y_padding) + shift, 1.5f, options_btn_size, Colour::theme_blue_1, Colour::theme_blue_2 + vec3(0.1), Screen::OPTIONS);

}

bool OptionsMenu::handle_step(float elapsed_ms_since_last_update, float current_speed) {
    std::stringstream title_ss;
    title_ss << "Harmonic Hustle options";
    glfwSetWindowTitle(window, title_ss.str().c_str());

    // Remove debug info from the last step
    while (registry.debugComponents.entities.size() > 0)
        registry.remove_all_components_of(registry.debugComponents.entities.back());

    // Render all button texts
    for (Entity entity : registry.boxButtons.entities) {
        if (registry.screens.get(entity) == Screen::OPTIONS) {
            BoxButton btn = registry.boxButtons.get(entity);
            Motion motion = registry.motions.get(entity);
            std::string text = registry.boxButtons.get(entity).text;
            vec3 text_colour = btn.text_colour;

            // if disabled, do a darker text 
            if (registry.disabled.has(entity)) {
                text_colour -= vec3(0.2);
            } else {
               // Hover effect
                // NOTE: if lag happens, comment this part out
                if ((text == "RESUME" && mouse_area == in_resume_btn) || (text == "CONTROLS" && mouse_area == in_ctrlsOpt_btn) || (text == "SAVE GAME" && mouse_area == in_save_btn) || (text == "NEW GAME" && mouse_area == in_new_game_btn)
                    || (text == "DIFFICULTY" && mouse_area == in_difficulty_btn) || (text == "TUTORIAL" && mouse_area == in_tutorial_btn) || (text == "MAIN MENU" && mouse_area == in_return_to_main_btn) || (text == "SAVE + EXIT" && mouse_area == in_exit_btn)) {
                    text_colour = Colour::white;
                } 
            }
            createText(text, motion.position, btn.text_scale, text_colour, Screen::OPTIONS, true, false);
        }
    }

    // Debug
    if (debugging.in_debug_mode) {
        for (Entity entity : registry.boxButtons.entities) {
            Motion motion = registry.motions.get(entity);
            vec2 pos = motion.position;
            BoxAreaBound bound = registry.boxAreaBounds.get(entity);
            createLine(vec2(bound.left, pos.y), vec2(5, motion.scale.y), Screen::OPTIONS);
            createLine(vec2(bound.right, pos.y), vec2(5, motion.scale.y), Screen::OPTIONS);
            createLine(vec2(pos.x, bound.top), vec2(motion.scale.x, 5), Screen::OPTIONS);
            createLine(vec2(pos.x, bound.bottom), vec2(motion.scale.x, 5), Screen::OPTIONS);
        }
    }

    return true;
}

bool OptionsMenu::set_visible(bool isVisible) {
    this->is_visible = isVisible;
    Entity& curr_screen_entity = registry.screenStates.entities[0];

    // remove curr screen component
    if (registry.screens.has(curr_screen_entity)) registry.screens.remove(curr_screen_entity);

    // if we need to set it to visible, add Start scene back
    if (is_visible) {
        registry.screens.insert(curr_screen_entity, Screen::OPTIONS);
    }

    ScreenState& screen = registry.screenStates.components[0];
    if (isVisible) {
        screen.darken_screen_factor = 0.2;
    }
    else {
        screen.darken_screen_factor = 0;
    }

    return is_visible;
}


bool OptionsMenu::disableButton(std::string buttonName) {
    if ((buttonName == "RESUME") && (!registry.disabled.has(resume_game_btn))) {
        vec3& btnColour = registry.colours.get(resume_game_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(resume_game_btn);

    } else if ((buttonName == "NEW GAME") && (!registry.disabled.has(new_game_btn))) {
        vec3& btnColour = registry.colours.get(new_game_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(new_game_btn);

    } else if ((buttonName == "SAVE GAME") && (!registry.disabled.has(save_game_btn))) {
        vec3& btnColour = registry.colours.get(save_game_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(save_game_btn);

    } else if ((buttonName == "DIFFICULTY") && (!registry.disabled.has(difficulty_btn))) {
        vec3& btnColour = registry.colours.get(difficulty_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(difficulty_btn);

    } else if ((buttonName == "CONTROLS") && (!registry.disabled.has(controls_btn))) {
        vec3& btnColour = registry.colours.get(controls_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(controls_btn);

    } else if ((buttonName == "TUTORIAL") && (!registry.disabled.has(tutorial_btn))) {
        vec3& btnColour = registry.colours.get(tutorial_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(tutorial_btn);

    } else if ((buttonName == "MAIN MENU") && (!registry.disabled.has(return_to_main_btn))) {
        vec3& btnColour = registry.colours.get(return_to_main_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(return_to_main_btn);

    } else if ((buttonName == "SAVE + EXIT") && (!registry.disabled.has(exit_btn))) {
        vec3& btnColour = registry.colours.get(exit_btn);
        btnColour -= vec3(0.2);
        registry.disabled.emplace(exit_btn);
    }
}

bool OptionsMenu::enableButton(std::string buttonName) {
if (buttonName == "RESUME") {
        vec3& btnColour = registry.colours.get(resume_game_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(resume_game_btn))
            registry.disabled.remove(resume_game_btn);

    } else if (buttonName == "NEW GAME") {
        vec3& btnColour = registry.colours.get(new_game_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(new_game_btn))
            registry.disabled.remove(new_game_btn);

    } else if (buttonName == "SAVE GAME") {
        vec3& btnColour = registry.colours.get(save_game_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(save_game_btn))
            registry.disabled.remove(save_game_btn);

    } else if (buttonName == "DIFFICULTY") {
        vec3& btnColour = registry.colours.get(difficulty_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(difficulty_btn))
            registry.disabled.remove(difficulty_btn);

    } else if (buttonName == "CONTROLS") {
        vec3& btnColour = registry.colours.get(controls_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(controls_btn))
            registry.disabled.remove(controls_btn);

    } else if (buttonName == "TUTORIAL") {
        vec3& btnColour = registry.colours.get(tutorial_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(tutorial_btn))
            registry.disabled.remove(tutorial_btn);

    } else if (buttonName == "MAIN MENU") {
        vec3& btnColour = registry.colours.get(return_to_main_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(return_to_main_btn))
            registry.disabled.remove(return_to_main_btn);

    } else if (buttonName == "SAVE + EXIT") {
        vec3& btnColour = registry.colours.get(exit_btn);
        btnColour = Colour::theme_blue_2 + vec3(0.1);
        if (registry.disabled.has(exit_btn))
            registry.disabled.remove(exit_btn);
    }
}


// Input callback functions
void OptionsMenu::handle_key(int key, int scancode, int action, int mod) {

}

void OptionsMenu::handle_mouse_move(MouseArea mouse_area) {
    this->mouse_area = mouse_area;
}
