#include "particle_generator.hpp"

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class SparkParticleGenerator : public ParticleGenerator
{
public:
    // subclass constructor
    // const int amount = 500;
    SparkParticleGenerator(GLuint shaderProgram, GLuint used_texture);

    PARTICLE_TYPE_ID getType();
private:
    void updateParticleBehaviours(Particle& p, float dt) override;
    void respawnParticle(Particle& particle, Entity entity, glm::vec2 offset) override;
};
