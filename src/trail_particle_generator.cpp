
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


TrailParticleGenerator::TrailParticleGenerator(GLuint shaderProgram, GLuint used_texture, int amount, int max_entities)
    : ParticleGenerator(shaderProgram, used_texture, amount, max_entities)
{
    // init is called in ParticleGenerator constructor
}

void TrailParticleGenerator::updateParticles(float dt, unsigned int newParticles, glm::vec2 offset) {
    for (int i = 0; i < max_entities; i++) {
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


void TrailParticleGenerator::updateParticleBehaviours(Particle& p, float dt, Entity entity) {
    // note that this only allows these particles to be attached to notes
    if (!registry.notes.has(entity) || !registry.particleEffects.has(entity)) {
        p.color.a = 0.f;
        p.life = 0.f;
        return;
    }

    Note& note = registry.notes.get(entity);
    p.life -= dt; // reduce life

    // particle is dead or below judgment line
    if (p.life <= 0.f) {
        p.color.a = 0.f;
        return;
    }

    // particle is alive, thus update
    p.position += p.velocity * dt;
    Motion& motion = registry.motions.get(entity);

    if (note.pressed && p.position.y >= 1 / 1.2f * gameInfo.height - gameInfo.judgment_line_half_height) {
        // Don't render below judgment line if note is being held
        p.color.a = 0.f;
    }
    else {
        float trail_extension_distance = -(conductor.crotchet * TRAIL_EXTENSION_MULTIPLIER);
        float distance_between_note_and_particle = motion.position.y - p.position.y;
        float duration_as_screen_distance = (note.duration / gameInfo.curr_note_travel_time * gameInfo.height);
        // Cover the cases where particle is immediately reassigned, and particle position is unpredictable
        if (distance_between_note_and_particle < 0.f 
            || distance_between_note_and_particle >= duration_as_screen_distance
            || abs(motion.position.x - p.position.x) > 50.f) {
            p.color.a = 0.f;
            return;
        }
        distance_between_note_and_particle += trail_extension_distance;
        float progress = min(distance_between_note_and_particle / duration_as_screen_distance, 1.f);
        float random = ((rand() % 100) - 50) * 2.f;
        p.color.a = lerp(1.f, 0.f, progress);
        if (note.pressed) {
            p.position += vec2(p.velocity.x + random, p.velocity.y) * 2.5f * dt;
        }
    }

    p.scale = vec2(DEFAULT_PARTICLE_SCALE * lerp(0.8f, 2.5f, p.position.y / gameInfo.height));

}

void TrailParticleGenerator::respawnParticle(Particle& particle, Entity entity, glm::vec2 offset)
{
    if (!registry.notes.has(entity) || !registry.particleEffects.has(entity)) {
        return;
    }
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 200) / 100.0f);
    Motion& entity_motion = registry.motions.get(entity);
    particle.position = entity_motion.position + random + offset;

    particle.color = glm::vec4(rColor, rColor, rColor, 0.8f);
    particle.color += gameInfo.particle_color_adjustment;
    particle.life = 1.f;
    particle.velocity = entity_motion.velocity * (0.1f * (additional_particles + 1));
    particle.scale = DEFAULT_PARTICLE_SCALE;
}

PARTICLE_TYPE_ID TrailParticleGenerator::getType() {
    return PARTICLE_TYPE_ID::TRAIL;
}

void TrailParticleGenerator::setAdditionalParticles(int num_particles) {
    additional_particles = num_particles;
}