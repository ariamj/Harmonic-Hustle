#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#include "render_system.hpp"
#include "audio_system.hpp"

struct NoteInfo {
    float spawn_time;
    int quantity;
};

struct BattleInfo {
    float bpm;
    float metadata_offset;
    int count_notes;
    std::vector<NoteInfo> notes;
    std::vector<std::pair<float, BattleMode>> modes;
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

        void init(GLFWwindow* window, RenderSystem* renderer, AudioSystem* audio, Serializer* saver);

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
        void handleRhythmInput(int action, int key);

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

        Serializer* saver;
    private:
        // Helpers for loading level data from file
        bool loadAllLevelData();
        bool loadLevelFromFile(int enemy_index);
        int convertDifficultyToInt(std::string difficulty);
        void convertBeatsToMilliseconds(std::vector<NoteInfo>* note_infos, float bpm_ratio);
        void convertBeatsToMilliseconds(std::vector<std::pair<float, BattleMode>>* mode_timings, float bpm_ratio);
        BattleMode convertStringToBattleMode(std::string mode_string);

        static const size_t MAX_NOTES = 10;
        static const int NUM_UNIQUE_BATTLES = 4;
        static const int NUM_DIFFICULTY_MODES = 2;

        const vec3 PERFECT_COLOUR = { 255.f, 1.f, 255.f };
        const vec3 GOOD_COLOUR = { 1.f, 255.f, 1.f };
        const vec3 ALRIGHT_COLOUR = { 255.f, 255.f, 1.f };
        const vec3 MISSED_COLOUR = { 255.f, 1.f, 1.f };

        const float APPROX_FRAME_DURATION = 16.6f;
        const float SCORING_LEEWAY = 1.2f * APPROX_FRAME_DURATION; // higher is easier to score better

        // Enemy-specific battle information
        BattleInfo battleInfo[NUM_UNIQUE_BATTLES * NUM_DIFFICULTY_MODES];

        // the time it should take for note to fall from top to bottom
        // TODO: Allow calibration via difficulty setting
        float note_travel_time = 2000.f;
        float spawn_offset; 

        // TODO: Allow calibration by player.
        float adjust_offset = 0.00f;
        float adjust_increment = 0.005f; // very small changes are impactful
        float timing_offset = 1 - (1.f / (1.2f + adjust_offset)); // coupled with judgment_y_pos in createJudgmentLine
        float top_to_judgment = note_travel_time * (1 - timing_offset); // time it takes from top edge to judgment lines

        // battle-specific variables for readability, initialized in .start
        int enemy_index;
        int num_notes;
        int next_note_index;
        int mode_index;
        float last_beat;
};