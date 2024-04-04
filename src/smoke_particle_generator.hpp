#include "particle_generator.hpp"

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class SmokeParticleGenerator : public ParticleGenerator
{
public:
    // subclass constructor
    // const int amount = 500;
    SmokeParticleGenerator(GLuint shaderProgram, GLuint used_texture);
    ~SmokeParticleGenerator() {};
private:
    PARTICLE_TYPE_ID getType() override;
    void updateParticles(float dt, unsigned int newParticles, glm::vec2 offset) override;
    void updateParticleBehaviours(Particle& p, float dt);
    void respawnParticle(Particle& particle, Entity entity, glm::vec2 offset);
};
