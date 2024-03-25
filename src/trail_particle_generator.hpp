#include "particle_generator.hpp"

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class TrailParticleGenerator : public ParticleGenerator
{
public:
    // subclass constructor
    // const int amount = 500;
    TrailParticleGenerator(GLuint shaderProgram, GLuint used_texture);
private:
    PARTICLE_TYPE_ID getType();
    void updateParticleBehaviours(Particle& p, float dt) override;
    void respawnParticle(Particle &particle, Entity entity, glm::vec2 offset) override;
};
