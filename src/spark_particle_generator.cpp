
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "spark_particle_generator.hpp"
#include "iostream"


SparkParticleGenerator::SparkParticleGenerator(GLuint shaderProgram, GLuint used_texture)
    : ParticleGenerator(shaderProgram, used_texture)
{
    // init is called in ParticleGenerator constructor
}

void SparkParticleGenerator::updateParticles(float dt, unsigned int newParticles, glm::vec2 offset) {
    for (int i = 0; i < MAX_PARTICLE_ENTITIES; i++) {
        Entity entity = blocks[i];

        if (entity == initialized_entity_id) {
            continue;
        }

        if (!registry.particleEffects.has(entity)) {
            // Clear the entity's previously assigned block of data
            // Note that this also sets scale, color to 0...
                // If particles not appearing, check that scale is set > 0, and color.a > 0
            memset(&particles[i * amount], 0.f, sizeof(Particle) * amount - 1);
            continue;
        }

        ParticleEffect& particle_effect = registry.particleEffects.get(entity);

        // add new particles 
        for (unsigned int i = 0; i < newParticles; ++i)
        {
            int unusedParticle = firstUnusedParticle(particle_effect.last_used_particle,
                particle_effect.min_index, particle_effect.max_index);
            particle_effect.last_used_particle = unusedParticle;
            respawnParticle(particles[unusedParticle], entity, TRAIL_NOTE_OFFSET);
        }

        // update all of a single entity's particles
        for (int i = particle_effect.min_index; i < particle_effect.max_index; i++)
        {
            updateParticleBehaviours(particles[i], dt, entity);
        }
    }
}


void SparkParticleGenerator::updateParticleBehaviours(Particle& p, float dt, Entity entity) {
    p.life -= dt; // reduce life
    if (p.life > 0.0f)
    {	// particle is alive, thus update
        p.position += p.velocity * dt * 5.f;
        p.color.a -= dt * 2.5;
        p.scale = vec2(DEFAULT_PARTICLE_SCALE * lerp(1.f, NOTE_MAX_SCALE_FACTOR, p.position.y / gameInfo.height));
    }
    else {
        // particle is dead, change alpha to hide rendering (dead particles are still rendered)
        p.color.a = 0.f;
    }
}

void SparkParticleGenerator::respawnParticle(Particle& particle, Entity entity, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    Motion& entity_motion = registry.motions.get(entity);
    particle.position = entity_motion.position + random + offset;

    particle.color = glm::vec4(rColor, rColor, rColor, 0.8f);
    particle.color += gameInfo.particle_color_adjustment;
    particle.life = 1.f;
    particle.velocity = entity_motion.velocity;
    particle.scale = DEFAULT_PARTICLE_SCALE;
}

PARTICLE_TYPE_ID SparkParticleGenerator::getType() {
    return PARTICLE_TYPE_ID::SPARK;
}
