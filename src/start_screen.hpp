#pragma once

// internal
#include "common.hpp"
#include "render_system.hpp"

class Start {
    public:
        Start();
        ~Start();

        void init(GLFWwindow* window, RenderSystem* renderer);

        bool handle_step(float elapsed_ms_since_last_update, float current_speed);

        bool set_visible(bool isVisible);
        bool is_visible;

        // Input callback functions
        void handle_key(int key, int scancode, int action, int mod);
        void handle_mouse_move(vec2 pos);

    private:
        // game state
        RenderSystem* renderer;
        GLFWwindow* window;
};