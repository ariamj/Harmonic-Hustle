
/*******************************************************************
** This code adapted from Breakout.
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

void SparkParticleGenerator::updateParticleBehaviours(Particle& p, float dt) {
    p.life -= dt; // reduce life
    if (p.life > 0.0f)
    {	// particle is alive, thus update
        float random = ((rand() % 100) - 50) / 2.0f;
        p.position += vec2(p.velocity.x + random, p.velocity.y) * dt * 3.f;
        p.color.a -= dt;
    }
    else {
        // particle is dead, change alpha to hide rendering (dead particles are still rendered)
        p.color.a = 0.f;
    }
}

void SparkParticleGenerator::respawnParticle(Particle& particle, Entity entity, glm::vec2 offset)
{
    if (!registry.particleEffects.has(entity)) {
        return;
    }
    ParticleEffect& effect = registry.particleEffects.get(entity);

    // Sparks should not continuously respawn like the trail particles did
    if (effect.spawned_particles >= effect.max_particles) {
        return;
    }

    effect.spawned_particles += 1;

    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    Motion& entity_motion = registry.motions.get(entity);
    particle.position = entity_motion.position + random + offset;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.color += gameInfo.battleModeColor;
    particle.life = 1.f;
    particle.velocity = entity_motion.velocity;
    particle.scale = DEFAULT_PARTICLE_SCALE;
}

PARTICLE_TYPE_ID SparkParticleGenerator::getType() {
    return PARTICLE_TYPE_ID::SPARK;
}
