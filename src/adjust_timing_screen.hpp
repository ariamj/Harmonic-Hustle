#pragma once

// internal
#include "common.hpp"
#include "render_system.hpp"

class AdjustTimingScreen {
public:
    AdjustTimingScreen();
    ~AdjustTimingScreen();

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
    Entity increase_timing_btn;
    Entity decrease_timing_btn;
    Entity back_to_options_btn;
    // mouse area
    MouseArea mouse_area;

private:
    // game state
    RenderSystem* renderer;
    GLFWwindow* window;
};