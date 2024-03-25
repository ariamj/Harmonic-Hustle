
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "trail_particle_generator.hpp"
#include "iostream"


TrailParticleGenerator::TrailParticleGenerator(GLuint shaderProgram, GLuint used_texture)
    : ParticleGenerator(shaderProgram, used_texture)
{
    // init is called in ParticleGenerator constructor
}

void TrailParticleGenerator::updateParticleBehaviours(Particle* p, float dt) {
    p->life -= dt; // reduce life
    if (p->life > 0.0f)
    {	// particle is alive, thus update
        p->position += p->velocity * dt;
        p->color.a -= dt * 2.5;
        p->scale = vec2(DEFAULT_PARTICLE_SCALE * lerp(1.f, 3.f, p->position.y / gameInfo.height));

    }
    else {
        // particle is dead, change alpha to hide rendering (dead particles are still rendered)
        p->color.a = 0.f;
    }
}

void TrailParticleGenerator::respawnParticle(Particle& particle, Entity entity, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    Motion& entity_motion = registry.motions.get(entity);
    particle.position = entity_motion.position + random + offset;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.life = 1.f;
    particle.velocity = entity_motion.velocity * 0.1f;
}

PARTICLE_TYPE_ID TrailParticleGenerator::getType() {
    return PARTICLE_TYPE_ID::TRAIL;
}
