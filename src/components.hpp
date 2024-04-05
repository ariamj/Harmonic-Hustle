#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include "screen.hpp"
#include "chrono"

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

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

struct Text
{
	// Text to be rendered
	std::string text;
	vec2 position = { 0.f, 0.f };
	float scale = 1.f;
	glm::vec3 colour = { 1.f, 1.f, 1.f };
	glm::mat4 trans = glm::mat4(1.f);
	Screen screen = Screen::OVERWORLD;
	bool center_pos;
};

struct BoxButton {
	Entity button_base;
	std::string text;
	float text_scale = 1.f;
	glm::vec3 text_colour = vec3(0.f);
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
	float spawn_time;
	float duration = -1.0f;
	float curr_duration = -1.0f;
};

struct NoteDuration
{
	float count_ms;
};

struct Player
{

};

struct Enemy
{

};

struct BattleEnemy {

};

struct BattlePlayer {

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
extern Conductor conductor;

// the area in which we gotta hit the notes
struct JudgementLine
{
	// png img ratios
	float actual_img_scale_factor = 1.f;
};

struct JudgementLineTimer
{
	float count_ms = 200;
};

struct GameInfo {
	Screen curr_screen;
	Screen prev_screen; // to handle resume correct screen after pausing
	std::shared_ptr<Entity> player_sprite;
	int height;
	int width;
	float lane_1;
	float lane_2;
	float lane_3;
	float lane_4;
	Entity curr_enemy;
	int curr_level = 1;
	int max_level = 4;
	bool victory = false; // True for testing; should be initialized to false
	bool is_boss_finished = false;
	bool is_intro_finished = false;
	bool is_game_over_finished = false;
	std::vector<std::vector<float>> existing_enemy_info; // contains vector of <posX, posY, level> of each enemy
	bool gameIsOver = false;
	vec4 particle_color_adjustment;
	float frames_adjustment = 0.f; // player-calibrated adjustment, measured in ms
};
extern GameInfo gameInfo;

// Battle specs for an enemy
struct BattleProfile {
	float score_threshold = 0.f;
};
struct Scene
{
	Screen scene;
};

struct PlayerCS {

};

struct EnemyCS {

};

struct CSText {

};

struct CSTextbox {

};

struct BattleCombo {

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

struct BattleReminderPopUp {

};

struct TutorialParts {

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
	HELP_BG = OVERWORLD_BG + 1,
	TRAIL_PARTICLE = HELP_BG + 1,
	BOSS_CS = TRAIL_PARTICLE + 1,
	BATTLEBOSS = BOSS_CS + 1,
	PLAYER_CS = BATTLEBOSS + 1,
	BOX_CS = PLAYER_CS + 1,
	SMOKE_PARTICLE = BOX_CS + 1,
	BATTLEENEMY_DRUM = SMOKE_PARTICLE + 1,
	BATTLEENEMY_MIC = BATTLEENEMY_DRUM + 1,
	BATTLEENEMY_DRUM_WIN = BATTLEENEMY_MIC + 1,
	BATTLEENEMY_MIC_WIN = BATTLEENEMY_DRUM_WIN + 1,
	BATTLEENEMY_GUITAR_WIN = BATTLEENEMY_MIC_WIN + 1,
	BATTLEBOSS_WIN = BATTLEENEMY_GUITAR_WIN + 1,
	BATTLEPLAYER_WIN = BATTLEBOSS_WIN + 1,
	BATTLEENEMY_DRUM_LOSE = BATTLEPLAYER_WIN + 1,
	BATTLEENEMY_MIC_LOSE = BATTLEENEMY_DRUM_LOSE + 1,
	BATTLEENEMY_GUITAR_LOSE = BATTLEENEMY_MIC_LOSE + 1,
	BATTLEBOSS_LOSE = BATTLEENEMY_GUITAR_LOSE + 1,
	BATTLEPLAYER_LOSE = BATTLEBOSS_LOSE + 1,
	NOTE_EXAMPLE_ABOVE = BATTLEPLAYER_LOSE + 1,
	NOTE_EXAMPLE_ON = NOTE_EXAMPLE_ABOVE + 1,
	NOTE_EXAMPLE_HIT = NOTE_EXAMPLE_ON + 1,
	FLAME_PARTICLE = NOTE_EXAMPLE_HIT + 1,
	SPARK_PARTICLE = FLAME_PARTICLE + 1,

	TEXTURE_COUNT = SPARK_PARTICLE +1 // keep as last variable
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
	FONT = NOTE + 1,
	TRAIL_PARTICLE = FONT + 1,
	SMOKE_PARTICLE = TRAIL_PARTICLE + 1,
	FLAME_PARTICLE = SMOKE_PARTICLE + 1,
	SPARK_PARTICLE = FLAME_PARTICLE + 1,
	EFFECT_COUNT = SPARK_PARTICLE + 1 // keep as last variable
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	// set geometry buffer IDs
	PLAYER = 0,
	SPRITE = PLAYER + 1,
	SCREEN_TRIANGLE = SPRITE + 1,
	DEBUG_LINE = SCREEN_TRIANGLE + 1,
	BOX = DEBUG_LINE + 1,
	CIRCLE_OUTLINE = BOX + 1,
	DOT = CIRCLE_OUTLINE + 1,
	FONT = DOT + 1,
	GEOMETRY_COUNT = FONT + 1 // keep as last variable
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

enum class PARTICLE_TYPE_ID {
	TRAIL = 0,
	SMOKE = TRAIL + 1,
	FLAME = SMOKE + 1,
	SPARK = FLAME + 1,
	PARTICLE_TYPE_COUNT = SPARK + 1
};
const int particle_type_count = (int)PARTICLE_TYPE_ID::PARTICLE_TYPE_COUNT;

struct ParticleEffect
{
	int last_used_particle;
	int min_index;
	int max_index;
	PARTICLE_TYPE_ID type;

	// for non-continuous-spawning particles 
	int spawned_particles = 0;
	int max_particles = 500; // can be set on entity creation
};

struct ParticleTimer
{
	float count_ms = 1500.f;
};

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

