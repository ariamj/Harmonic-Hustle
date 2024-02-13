#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

// #define SDL_MAIN_HANDLED
// #include <SDL.h>
// #include <SDL_mixer.h>

#include "render_system.hpp"

class Overworld {
    public:
        Overworld();
        bool is_visible;

        void init(GLFWwindow* window, RenderSystem* renderer);

        // Releases all associated resources
        ~Overworld();

        // Steps the game ahead by ms milliseconds
        bool handle_step(float elapsed_ms_since_last_update);

        bool set_visible(bool isVisible);

        // Check for collisions
        void handle_collisions();

        // Should the game be over ?
        // bool is_over()const;
    // private:
        // Input callback functions
        void handle_key(int key, int scancode, int action, int mod);
        void handle_mouse_move(vec2 pos);

        // restart level
        void handle_reset();

        // OpenGL window handle
        // GLFWwindow* window;

        // Game state
        // RenderSystem* renderer;

        // music references
        // Mix_Music* background_music;

        // C++ random number generator
        std::default_random_engine rng;
        std::uniform_real_distribution<float> uniform_dist; // number between 0..1

        // game state
        RenderSystem* renderer;
        float current_speed;
        float next_enemy_spawn;
        Entity player_sprite;

        GLFWwindow* window;
};