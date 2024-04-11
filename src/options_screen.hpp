#pragma once

// internal
#include "common.hpp"
#include "render_system.hpp"

class OptionsMenu {
public:
    OptionsMenu();
    ~OptionsMenu();

    void init(GLFWwindow* window, RenderSystem* renderer);
    void init_screen();
    void renderButtons();

    bool handle_step(float elapsed_ms_since_last_update, float current_speed);

    bool set_visible(bool isVisible);
    bool is_visible;

    // Input callback functions
    void handle_key(int key, int scancode, int action, int mod);
    void handle_mouse_move(MouseArea mouse_area);

    // buttons
    Entity resume_game_btn;
    Entity adjust_timing_btn;
    Entity new_game_btn;
    Entity difficulty_btn;
    Entity controls_btn;
    Entity tutorial_btn;
    Entity exit_btn;
    Entity return_to_main_btn;
    // mouse area
    MouseArea mouse_area;

    // only disables and enables buttons by updating the button colour
    //      check if button should actually disabled before using
    bool disableButton(std::string buttonName);
    bool enableButton(std::string buttonName);

private:
    // game state
    RenderSystem* renderer;
    GLFWwindow* window;

    vec2 title_1_pos;
};