
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

void TrailParticleGenerator::Update(float dt, unsigned int newParticles, glm::vec2 offset)
{
    // Check for new entities with ParticleEffect since last update
    for (auto entity : registry.particleEffects.entities) {
        // Check if entity is current assigned to a block
        bool entity_found = false;
        for (int i = 0; i < MAX_PARTICLE_ENTITIES; i++) {
            if (blocks[i] == entity) {
                entity_found = true;
            }
        }
        // Assign entity to a block and initialize values
        if (!entity_found) {
            ParticleEffect& particle_effect = registry.particleEffects.get(entity);
            int free_block = findUnusedBlock();
            // if no free blocks were found, skip
            if (free_block == -1) {
                continue;
            }
            blocks[free_block] = entity;

            particle_effect.min_index = free_block * amount;
            particle_effect.max_index = particle_effect.min_index + amount - 1;
            particle_effect.last_used_particle = particle_effect.min_index;
        }
    }
    
    int count = 0;
    for (int i = 0; i < MAX_PARTICLE_ENTITIES; i++) {
        Entity entity = blocks[i];

        if (entity == initialized_entity_id) {
            continue;
        }

        if (!registry.particleEffects.has(entity) ) {
            // Clear block of particle data
            memset(&particles[i * amount], 0.f, sizeof(Particle) * amount - 1);
            // OPTIMIZE: flag deleted blocks
            // blocks[i] = 0 does not compile in current implementation
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
        // update this entity's particles
        for (int i = particle_effect.min_index; i < particle_effect.max_index; i++)
        {
            Particle* p = &particles[i];
            p->life -= dt; // reduce life
            if (p->life > 0.0f)
            {	// particle is alive, thus update
                p->position += p->velocity * dt;
                p->color.a -= dt * 2.5;
                p->scale = vec2(DEFAULT_PARTICLE_SCALE * lerp(1.f, 3.f, p->position.y / gameInfo.height));
                count += 1;
            }
            else {
                // particle is dead, change alpha to hide rendering (dead particles are still rendered)
                p->color.a = 0.f;
            }
        }
        std::cout << "DT:" << dt * 1000.f << "\n";
    }
    count = 0;
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
