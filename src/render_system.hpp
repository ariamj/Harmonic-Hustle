#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

#include "trail_particle_generator.hpp"
#include "smoke_particle_generator.hpp"
#include "flame_particle_generator.hpp"
#include "spark_particle_generator.hpp"

#include <map>

struct CharacterRequest {
	float vertices[6][4];
	unsigned int TextureID;
};

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::PLAYER, mesh_path("chicken.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SPRITE, mesh_path("Player-Walk-F1.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SPRITE, mesh_path("Player-Walk-F2.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SPRITE, mesh_path("Player-Walk-F3.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
		textures_path("Player-Walk-F1.png"),
		textures_path("Player-Walk-F2.png"),
		textures_path("Player-Walk-F3.png"),
		textures_path("Enemy-Guitar.png"),
		textures_path("Enemy-Drum.png"),
		textures_path("Enemy-Mic.png"),
		textures_path("Player-Portrait-Neutral.png"),
		textures_path("Enemy-Guitar-Portrait.png"),
		textures_path("Judgement.png"),
		textures_path("Note.png"),
		textures_path("Overworld-Background.png"),
		textures_path("Help-Screen-Background.png"),
		textures_path("Trail-Particle.png"),
		textures_path("Boss-CS-Neutral.png"),
		textures_path("Boss-Portrait-Neutral.png"),
		textures_path("Player-CS-Neutral.png"),
		textures_path("CS-Text-Box.png"),
		textures_path("Smoke-Particle.png"),
		textures_path("Enemy-Drum-Portrait.png"),
		textures_path("Enemy-Mic-Portrait.png"),
		textures_path("Enemy-Drum-Portrait-Win.png"),
		textures_path("Enemy-Mic-Portrait-Win.png"),
		textures_path("Enemy-Guitar-Portrait-Win.png"),
		textures_path("Boss-Portrait-Win.png"),
		textures_path("Player-Portrait-Win.png"),
		textures_path("Enemy-Drum-Portrait-Lose.png"),
		textures_path("Enemy-Mic-Portrait-Lose.png"),
		textures_path("Enemy-Guitar-Portrait-Lose.png"),
		textures_path("Boss-Portrait-Lose.png"),
		textures_path("Player-Portrait-Lose.png"),
		textures_path("Note-Example-Above.png"),
		textures_path("Note-Example-On.png"),
		textures_path("Note-Example-Hit.png"),
		textures_path("Flame-Particle.png"),
		textures_path("Spark-Particle.png"),
		textures_path("Judgement-Lines-Example.png"),
		textures_path("Long-Note-Example.png"),
		textures_path("Tap-Note-Example.png"),
		textures_path("Held-Note-Example-Start.png"),
		textures_path("Held-Note-Example-Tap.png"),
		textures_path("Held-Note-Example-Holding.png"),
		textures_path("Held-Note-Example-Finish.png"),
		textures_path("Mode-Back-And-Forth.png"),
		textures_path("Mode-Beat-Rush.png"),
		textures_path("Mode-Unison.png"),
		textures_path("Mode-Change-Start.png"),
		textures_path("Mode-Change-Mid.png"),
		textures_path("Mode-Change-Finish.png"),
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("playerSprite"),
		shader_path("enemySprite"),
		shader_path("battlePlayerSprite"),
		shader_path("battleEnemySprite"),
		shader_path("coloured"),
		shader_path("textured"),
		shader_path("environment"),
		shader_path("judgement"),
		shader_path("note"),
		shader_path("font"),
		shader_path("trailParticle"),
		shader_path("smokeParticle"),
		shader_path("flameParticle"),
		shader_path("sparkParticle")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	std::map<char, Character> m_ftCharacters;

	// Initialize the window
	bool init(GLFWwindow* window);

    bool fontInit(GLFWwindow& window, const std::string& font_filename, unsigned int font_default_size);

    template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	void initializeParticleGenerators();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& trans = glm::mat4(1.f), bool center_pos = true);

	mat3 createProjectionMatrix();

	// Particles
	std::vector<std::shared_ptr<ParticleGenerator>> particle_generators;

	void createParticleGenerator(int particle_type_id, int additional_particles = 0);


private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	GLuint vao;
	GLuint m_font_VAO;
	GLuint m_font_VBO;
	GLuint m_font_shaderProgram;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
