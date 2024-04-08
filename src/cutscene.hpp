#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#include "render_system.hpp"

class Cutscene {
    public:
        int boss_dialogue_progress = 0;
        int intro_dialogue_progress = 0;
        int game_over_dialogue_progress = 0;
        int lose_boss_dialogue_progress = 0;

        const std::vector<std::string> BOSS_DIALOGUE = {
            "you're gonna regret not turning off that music",
            "this party will forever go on",
            "we'll see about that",
            "ya we will see about that..."
        };

        const std::vector<std::string> INTRO_DIALOGUE = {
            "you've had that music blasting all night, can you turn it off?!",
            "chill out bro, it's not even that late",
            "this is the last time I'm gonna ask, turn it off or I'm coming over there!!!",
            "kick rocks lady",
            "alright thats it!!"
        };

        const std::vector<std::string> GAME_OVER_DIALOGUE = {
            "HA! how do you feel now that i've defeated you and all your minions??",
            "...honestly just tired. you win. enjoy your moment of peace for now...",
            "...Hey, what do you mean for now??",
            "You realize it's only been one night right?",
            "...",
            "See you tomorrow :)"
        };

        const std::vector<std::string> LOSE_BOSS_DIALOGUE = {
            "...",
            "HA! Did you think you could defeat me?! Think again!",
            "This isn't over! Rematch!!",
            "...seriously?...fine..."
        };

        const std::string CONT_TEXT = "PRESS SPACE TO CONTINUE";

        Cutscene();
        bool is_visible;

        void remove_prev_assets();

        void init(GLFWwindow* window, RenderSystem* renderer);

        // Releases all associated resources
        ~Cutscene();

        void init_screen();

        // Steps the game ahead by ms milliseconds
        bool handle_step(float elapsed_ms_since_last_update, float current_speed);

        bool set_visible(bool isVisible);

        // Check for collisions
        // void handle_collisions();

        // Input callback functions
        void handle_key(int key, int scancode, int action, int mod);
        void handle_mouse_move(vec2 pos);

        // C++ random number generator
        std::default_random_engine rng;
        std::uniform_real_distribution<float> uniform_dist; // number between 0..1

         // game state
        RenderSystem* renderer;
        GLFWwindow* window;
};