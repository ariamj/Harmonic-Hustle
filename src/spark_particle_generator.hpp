#include "particle_generator.hpp"

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class SparkParticleGenerator : public ParticleGenerator
{
public:
    // subclass constructor
    // const int amount = 500;
    SparkParticleGenerator(GLuint shaderProgram, GLuint used_texture, int amount, int max_entities);
    ~SparkParticleGenerator() {};
private:
    PARTICLE_TYPE_ID getType() override;
    void updateParticles(float dt, unsigned int newParticles, glm::vec2 offset) override;
    void updateParticleBehaviours(Particle& p, float dt, Entity entity);
    void respawnParticle(Particle& particle, Entity entity, glm::vec2 offset);
    vec2 trace(vec3 rpos, vec3 rdir, vec2 fragCoord, float life);
};
