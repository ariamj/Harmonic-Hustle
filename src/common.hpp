#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <string>
#include <tuple>
#include <vector>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
using namespace glm;

#include "tiny_ecs.hpp"
#include "serializer.hpp"

// Simple utility functions to avoid mistyping directory name
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "../ext/project_path.hpp"
inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string& name) {return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) {return data_path() + "/textures/" + std::string(name);};
inline std::string audio_path(const std::string& name) {return data_path() + "/audio/" + std::string(name);};
inline std::string mesh_path(const std::string& name) {return data_path() + "/meshes/" + std::string(name);};
inline std::string saves_path(const std::string& name) { return data_path() + "/saves/" + std::string(name); };
inline std::string levels_path(const std::string& name) { return data_path() + "/levels/" + std::string(name); };

// const int window_width_px = 1680;
// const int window_height_px = 1050;

// These are hard coded to the dimensions of the entity texture
const float PLAYER_WIDTH = 1.0f * 165.f;
const float PLAYER_HEIGHT = 1.0f * 165.f;
const float ENEMY_WIDTH = 1.0f * 165.f;
const float ENEMY_HEIGHT = 1.0f * 165.f;
const float PORTRAIT_WIDTH = 3 * 150.f;
const float PORTRAIT_HEIGHT = 3 * 150.f;
const float CS_WIDTH = 850.f;
const float CS_HEIGHT = 1350.f;
const int MAX_TEXT_LENGTH = 255;
const float NOTE_WIDTH = 0.6f * 165.f;
const float NOTE_HEIGHT = 0.6f * 165.f;
const float NOTE_MAX_SCALE_FACTOR = 2.5f;
// const float CHASE_PLAYER_RADIUS = 300.f;
// const float RUN_AWAY_RADIUS = 300.f;
const float PLAYER_ENEMY_RADIUS = 300.f;
const float CHASE_TOTAL_VELOCITY = 50.f;
const float RUN_AWAY_TOTAL_VELOCITY = 50.f;
const float DEFAULT_ENEMY_VELOCITY = 50.f;
const float X_DISPLACEMENT_PORTRAIT = PORTRAIT_WIDTH * 2.5f / 7.f;
const float Y_DISPLACEMENT_PORTRAIT = PORTRAIT_HEIGHT / 2;

// Scaling to editors' screens
const float ORIGINAL_MONITOR_WIDTH = 2256.f;
const float ORIGINAL_MONITOR_HEIGHT = 1504.f;
const float K_MONITOR_WIDTH = 1470.f;
const float K_MONITOR_HEIGHT = 956.f;

// Particle-related
const vec2 DEFAULT_PARTICLE_SCALE = vec2(10.f);
const vec2 TRAIL_NOTE_OFFSET = vec2(-NOTE_WIDTH / 10, -NOTE_HEIGHT / 3.5f);
const int MAX_PARTICLE_ENTITIES = 10; // coupled with notes for now
const int BASE_NEW_PARTICLES = 2;
const int MAX_FPS_ON_MAC = 125;

// for making notes travel faster on higher-level enemy collision
const float NOTE_TRAVEL_TIME_MULTIPLER = 0.75f;

// FPS global set by main.cpp
extern int FPS;

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
// We recomment making all components non-copyable by derving from ComponentNonCopyable
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

// Commonly used colours (especially for texts)
namespace Colour {
	static const vec3 red = { 1.0, 0.0, 0.0 };
	static const vec3 dark_red = red - vec3(0.5f);
	static const vec3 yellow = { 1.0, 1.0, 0.0 };
	static const vec3 dark_yellow = yellow - vec3(0.5f);
	static const vec3 green = { 0.0, 1.0, 0.0 };
	static const vec3 dark_green = green - vec3(0.5f);
	static const vec3 blue = { 0.0, 0.0, 1.0 };
	static const vec3 dark_blue = blue - vec3(0.5f);
	static const vec3 purple = { 1.0, 0.0, 1.0 };
	static const vec3 dark_purple = purple - vec3(0.5f);
	static const vec3 black = { 0.0, 0.0, 0.0 };
	static const vec3 light_gray = { 0.75, 0.75, 0.75 };
	static const vec3 gray = { 0.5, 0.5, 0.5 };
	static const vec3 dark_gray = { 0.25, 0.25, 0.25 };
	static const vec3 white = { 1.0, 1.0, 1.0 };
	static const vec3 khaki = vec3(240,230,140) / vec3(255);
	static const vec3 off_white = vec3(0.8, 0.83, 0.851);
	static const vec3 red_enemy_tint = vec3{0.95f, 0.6f, 0.6f};
	static const vec3 lane_color = { 0.03922, 0.13725, 0.15686 };

	// game theme colours
	static const vec3 theme_blue_1 = { 0.758, 0.784, 0.801 }; //lighter blue
	static const vec3 theme_blue_2 = { 0.308, 0.434, 0.451 }; //mid blue
	static const vec3 theme_blue_3 = { 0.048, 0.184, 0.201 }; //background colour

	// mode and particle colours
	static const vec3 back_and_forth_colour = { 0.153, 0.6098, 0.6169};
	static const vec3 beat_rush_colour = { 0.65, 0.015, 0.015 };
	static const vec3 unison_colour = { 0.709, 0.376, 0.1059 };
	
};

// box area of interest (eg. for buttons, active area)
struct BoxAreaBound {
	float left;
	float right;
	float top;
	float bottom;
};

// Button boundary enums
enum MouseArea {
	in_unclickable,
	in_start_btn,
	in_settings_btn,
	in_restart_btn,
	in_load_btn,
	in_easy_btn,
	in_normal_btn,
	in_hard_btn,
	in_resume_btn,
	in_adjust_timing_btn,
	in_new_game_btn,
	in_difficulty_btn,
	in_ctrlsOpt_btn,
	in_tutorial_btn,
	in_exit_btn,
	in_return_to_main_btn,
	in_increase_frame_btn,
	in_decrease_frame_btn,
	in_back_btn,
	in_x_btn,

};

// Modes of note timings
enum BattleMode {
    back_and_forth = 1, // listen and copy
    beat_rush = 2, // non-stop equal separation
    unison = 3	// press at the same time as you hear it.
				// player should have encountered rhythm before
};

bool gl_has_errors();

vec2 getRandomEnemyVelocity();

float lerp(float start, float end, float t);

// for adjusting note timings
const float MIN_FRAMES_ADJUSTMENT = -5.f;
const float MAX_FRAMES_ADJUSTMENT = 5.f;