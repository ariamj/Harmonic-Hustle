#pragma once

// internal
#include "common.hpp"
#include "render_system.hpp"

class GameOver {
    public:
        GameOver();
        ~GameOver();

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
        Entity restart_btn;
	    Entity help_btn;

        // mouse area
        MouseArea mouse_area;

    private:
        // game state
        RenderSystem* renderer;
        GLFWwindow* window;

        vec2 title_1_pos;
        vec2 title_2_pos;
};