#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#include "render_system.hpp"

class Battle {
    public:
        Battle();
        bool is_visible;

        void init(GLFWwindow* window, RenderSystem* renderer);

        // Releases all associated resources
        ~Battle();

        // Steps the game ahead by ms milliseconds
        bool handle_step(float elapsed_ms_since_last_update, float current_speed);

        bool set_visible(bool isVisible);

        // Check for collisions
        void handle_collisions();

        // Input callback functions
        void handle_key(int key, int scancode, int action, int mod);
        void handle_mouse_move(vec2 pos);

        // C++ random number generator
        std::default_random_engine rng;
        std::uniform_real_distribution<float> uniform_dist; // number between 0..1

         // game state
        RenderSystem* renderer;
        //float current_speed;
        float next_note_spawn;
        Entity player_battle_sprite;
        Entity enemy_battle_sprite;

        GLFWwindow* window;
};