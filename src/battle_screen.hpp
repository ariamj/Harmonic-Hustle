#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#include "render_system.hpp"
#include "audio_system.hpp"

struct BattleInfo {
    float bpm;
    int count_notes;
    std::vector<float> note_timings;
};

class Battle {
    public:
        Battle();
        bool is_visible;
        enum Standing {
            perfect = 15,
            good = 10,
            alright = 5,
            missed = -5
        };

        void init(GLFWwindow* window, RenderSystem* renderer, AudioSystem* audio);

        // Releases all associated resources
        ~Battle();

        // Steps the game ahead by ms milliseconds
        bool handle_step(float elapsed_ms_since_last_update, float current_speed);

        void handle_battle_end();

        // Setters
        void start();

        bool set_pause(bool isPaused);

        bool set_visible(bool isVisible);

        void setBattleIsOver(bool isOver);
        bool battleWon();

        // Check for collisions
        void handle_collisions();
        void handle_note_hit(Entity entity, Entity entity_other);

        // Input callback functions
        void handle_key(int key, int scancode, int action, int mod);
        void handle_mouse_move(vec2 pos);

        // C++ random number generator
        std::default_random_engine rng;
        std::uniform_real_distribution<float> uniform_dist; // number between 0..1

         // game state
        RenderSystem* renderer;
        AudioSystem* audio;
        //float current_speed;
        float next_note_spawn;
        Entity player_battle_sprite;
        Entity enemy_battle_sprite;
        bool d_key_pressed = false;
        bool f_key_pressed = false;
        bool j_key_pressed = false;
        bool k_key_pressed = false;
        Standing standing;
        int perfect_counter = 0;
        int good_counter = 0;
        int alright_counter = 0;
        int missed_counter = 0;
        float score = 0;
        float score_threshold = 200.f; // TODO: load info from enemy battle sprite

        float lanes[4] = {0, 0, 0, 0};
        Entity gameOverPopUpOverlay;

        bool battle_is_over = false;
        
        // set to true once player resumes game from pause
        bool in_countdown = false;
        float countdownTimer_ms;

        GLFWwindow* window;
};