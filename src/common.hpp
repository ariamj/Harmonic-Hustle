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

// Simple utility functions to avoid mistyping directory name
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "../ext/project_path.hpp"
inline std::string data_path() { return std::string(PROJECT_SOURCE_DIR) + "data"; };
inline std::string shader_path(const std::string& name) {return std::string(PROJECT_SOURCE_DIR) + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) {return data_path() + "/textures/" + std::string(name);};
inline std::string audio_path(const std::string& name) {return data_path() + "/audio/" + std::string(name);};
inline std::string mesh_path(const std::string& name) {return data_path() + "/meshes/" + std::string(name);};

// const int window_width_px = 1680;
// const int window_height_px = 1050;

// These are hard coded to the dimensions of the entity texture
const float PLAYER_WIDTH = 1.0f * 165.f;
const float PLAYER_HEIGHT = 1.0f * 165.f;
const float ENEMY_WIDTH = 1.0f * 165.f;
const float ENEMY_HEIGHT = 1.0f * 165.f;
const float PORTRAIT_WIDTH = 3 * 165.f;
const float PORTRAIT_HEIGHT = 3 * 165.f;
const float CS_WIDTH = 850.f;
const float CS_HEIGHT = 1350.f;
const float NOTE_WIDTH = 0.6f * 165.f;
const float NOTE_HEIGHT = 0.6f * 165.f;
// const float CHASE_PLAYER_RADIUS = 300.f;
// const float RUN_AWAY_RADIUS = 300.f;
const float PLAYER_ENEMY_RADIUS = 300.f;
const float CHASE_TOTAL_VELOCITY = 50.f;
const float RUN_AWAY_TOTAL_VELOCITY = 50.f;
const float DEFAULT_ENEMY_VELOCITY = 50.f;


// Interpolation constants, on range [0,1] 
const float NOTE_POSITION_STEP_SIZE = 0.002f;

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
};

bool gl_has_errors();

vec2 getRandomEnemyVelocity();
