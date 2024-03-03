#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include "screen.hpp"


// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// Single Vertex Buffer element for non-textured meshes
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0.f;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	float scale_factor = 1.f;
	float progress = 0.f; // range [0,1] for linear interpolation
};

// for notes
struct Note
{
	bool pressed = false;
};

struct Player
{

};

struct Enemy
{

};

// for keeping beat
// https://fizzd.notion.site/How-To-Make-A-Rhythm-Game-Technical-Guide-ed09f5e09752451f97501ebddf68cf8a
struct Conductor
{
	float bpm;
	float crotchet;
	float offset;
	float song_position;
};

// the area in which we gotta hit the notes
struct JudgementLine
{

};

struct JudgementLineTimer
{
	float count_ms = 200;
};

// used to manage the different screens (????)
// the scene that the entity exists in
struct GameInfo {
	Screen curr_screen;
	std::shared_ptr<Entity> player_sprite;
	int height;
	int width;
	float lane_1;
	float lane_2;
	float lane_3;
	float lane_4;
};
extern GameInfo gameInfo;
struct Scene
{
	Screen scene;
};

struct IsChasing {
	
};

struct IsRunning {

};

struct Level {
	int level;
};

// pauses all enemy movement & collisions -> add to player for usage
struct PauseEnemyTimer {
	float counter_ms = 1500;
};

// if part of the battle over popup
struct BattleOverPopUp {
	
};

struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// how many seconds you have to react to the collision
struct CollisionTimer {
	float counter_ms = 1000;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	PLAYER_WALK_F1 = 0,
	PLAYER_WALK_F2 = PLAYER_WALK_F1 + 1,
	PLAYER_WALK_F3 = PLAYER_WALK_F2 + 1,
	ENEMY_GUITAR = PLAYER_WALK_F3 + 1,
	ENEMY_DRUM = ENEMY_GUITAR + 1,
	ENEMY_MIC = ENEMY_DRUM + 1,
	BATTLEPLAYER = ENEMY_MIC + 1,
	BATTLEENEMY = BATTLEPLAYER + 1,
	JUDGEMENT = BATTLEENEMY + 1,
	NOTE = JUDGEMENT + 1,
	OVERWORLD_BG = NOTE + 1,
	TEXTURE_COUNT = OVERWORLD_BG + 1 // keep as last variable
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	// set effect asset IDs
	PLAYER = 0,
	ENEMY_GUITAR = PLAYER + 1,
	BATTLEPLAYER = ENEMY_GUITAR + 1,
	BATTLEENEMY = BATTLEPLAYER + 1,
	COLOURED = BATTLEENEMY + 1,
	TEXTURED = COLOURED + 1,
	ENVIRONMENT = TEXTURED + 1,
	JUDGEMENT = ENVIRONMENT + 1,
	NOTE = JUDGEMENT + 1,
	EFFECT_COUNT = NOTE + 1 // keep as last variable
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	// set geometry buffer IDs
	PLAYER = 0,
	SPRITE = PLAYER + 1,
	SCREEN_TRIANGLE = SPRITE + 1,
	DEBUG_LINE = SCREEN_TRIANGLE + 1,
	BOX = DEBUG_LINE + 1,
	GEOMETRY_COUNT = BOX + 1 // keep as last variable
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

