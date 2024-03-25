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
    //     bool is_boss_finished = false;
    //     bool is_intro_finished = false;
    //     bool is_game_over_finished = false;

        const std::string BOSS_DIALOGUE[4] = {
            "you're gonna regret not turning off that music",
            "this party will forever go on",
            "we'll see about that",
            "ya we will see about that..."
        };

        const std::string INTRO_DIALOGUE[5] = {
            "you've had that music blasting all night, can you turn it off?!",
            "chill out bro, it's not even that late",
            "this is the last time I'm gonna ask, turn it off or I'm coming over there!!!",
            "kick rocks lady",
            "alright thats it!!"
        };

        const int GAME_OVER_DIALOGUE_SENTENCES = 6;

        const std::string GAME_OVER_DIALOGUE[6] = {
            "I've done it! Now that you're defeated, GO AWAY!",
            "I see that you've finally won against me... You can enjoy this moment of peace for now...",
            "Hey, what do you mean for now?",
            "You realize it's only been a night right?",
            "...",
            "See you tomorrow :)"
        };

        const std::string CONT_TEXT = "PRESS SPACE TO CONTINUE";

        Cutscene();
        bool is_visible;

        void remove_prev_assets();

        void init(GLFWwindow* window, RenderSystem* renderer);

        // Releases all associated resources
        ~Cutscene();

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