#include "particle_generator.hpp"

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class TrailParticleGenerator : public ParticleGenerator
{
public:
    // subclass constructor
    // const int amount = 500;
    TrailParticleGenerator(GLuint shaderProgram, GLuint used_texture, int amount, int max_entities);
    ~TrailParticleGenerator() {};
    PARTICLE_TYPE_ID getType() override;
    void setAdditionalParticles(int num_particles);
private:
    // Extend trail to appear longer
    int additional_particles = 0;
    const float TRAIL_EXTENSION_MULTIPLIER = 0.05f; // percentage of a beat to extend trail by
    void updateParticles(float dt, unsigned int newParticles, glm::vec2 offset) override;
    void updateParticleBehaviours(Particle& p, float dt, Entity entity);
    void respawnParticle(Particle &particle, Entity entity, glm::vec2 offset);
};
