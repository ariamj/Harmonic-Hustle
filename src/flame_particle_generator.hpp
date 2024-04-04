#include "particle_generator.hpp"

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class FlameParticleGenerator : public ParticleGenerator
{
public:
    // subclass constructor
    // const int amount = 500;
    FlameParticleGenerator(GLuint shaderProgram, GLuint used_texture);
    ~FlameParticleGenerator() {};
private:
    const float ALPHA_FADE_MULTIPLIER = 2.5f;
    PARTICLE_TYPE_ID getType() override;
    void updateParticles(float dt, unsigned int newParticles, glm::vec2 offset) override;
    void updateParticleBehaviours(Particle& p, float dt, Entity entity);
    void respawnParticle(Particle& particle, Entity entity, glm::vec2 offset);
};
