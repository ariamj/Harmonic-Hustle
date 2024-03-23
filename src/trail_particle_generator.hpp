#include "particle_generator.hpp"

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class TrailParticleGenerator : public ParticleGenerator
{
public:
    // subclass constructor
    TrailParticleGenerator(GLuint shaderProgram, TEXTURE_ASSET_ID used_texture, Entity entity);
    void Update(float dt, unsigned int newParticles, glm::vec2 offset);
    void respawnParticle(Particle& particle, glm::vec2 offset);
};
