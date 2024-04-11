#include "render_system.hpp"

enum TutorialPart {
    INTRO = 0, // how to navigate overworld, explain enemy types
    BATTLE_EXPLAIN = 1, // note explanation, how to win
    ADVANCING_EXPLAIN = 2, // explain levels, boss, difficulty, difficulty buttons
    CHOOSE_DIFFICULTY = 3,
    ADJUST_TIMING = 4,
    NUM_OF_PARTS = 5
};

class Tutorial {
    public:

        int tutorial_progress = TutorialPart::INTRO; // number of tutorial parts
        int tutorial_max_progress_parts = TutorialPart::NUM_OF_PARTS; // max number of parts

        Tutorial();
        ~Tutorial();

        void init(GLFWwindow* window, RenderSystem* renderer);
        void init_parts(TutorialPart part);

        bool handle_step(float elapsed_ms_since_last_update, float current_speed);

        bool set_visible(bool isVisible);
        bool is_visible;

         // Input callback functions
        void handle_key(int key, int scancode, int action, int mod);
        void handle_mouse_move(MouseArea mouse_area);
        void handle_difficulty_click(int difficulty);

        Entity easy_btn;
	    Entity normal_btn;
        Entity hard_btn;
        Entity x_btn;

        // frames adjustment buttons
        Entity increase_frames_btn;
        Entity decrease_frames_btn;

        // text displaying frame adjustment
        Entity curr_timing_text;

        // mouse area
        MouseArea mouse_area;

    private:

        void initIntroParts();
        void initBattleExplainParts();
        void initAdvancingExplaingParts();
        void initChooseDifficultyParts();
        void initAdjustNoteTimingParts();

        // game state
        RenderSystem* renderer;
        GLFWwindow* window;
};