
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "particle_generator.hpp"
#include "iostream"

ParticleGenerator::ParticleGenerator(GLuint shaderProgram, GLuint used_texture)
    : shaderProgram(shaderProgram), used_texture(used_texture)
{
    init();
}

void ParticleGenerator::Update(float dt, unsigned int newParticles, glm::vec2 offset)
{
    updateEntities();

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
            updateParticleBehaviours(particles[i], dt);
        }
    }
    return;
}

void ParticleGenerator::updateEntities() {
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

            // Verify that particle_effect matches this generator's type
            if (particle_effect.type != getType()) {
                continue;
            }

            int free_block = findUnusedBlock();
            // if no free blocks were found, skip
            if (free_block == -1) {
                continue;
            }

            // set entity reference, and indices of shared particles array
            blocks[free_block] = entity;
            particle_effect.min_index = free_block * amount;
            particle_effect.max_index = particle_effect.min_index + amount - 1;
            particle_effect.last_used_particle = particle_effect.min_index;
        }
    }
}

void ParticleGenerator::updateParticleBehaviours(Particle& p, float dt) {
    std::cout << "WARNING: Base class ParticleGenerator::updateParticleBehaviours has been called" << "\n";
    return; // should be overridden in subclasses
}

// render all particles
void ParticleGenerator::Draw()
{
    // don't draw particles if in settings screen (update later if needed)
    if (gameInfo.curr_screen != Screen::SETTINGS) {
        // use additive blending to give it a 'glow' effect
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glUseProgram(shaderProgram);

        // Previous non-instanced rendering
        // for (Particle particle : particles)
        // {
        //     if (particle.life > 0.0f)
        //     {
        //         //this->shader.SetVector2f("offset", particle.Position);
        //         GLint offset_uloc = glGetUniformLocation(shaderProgram, "offset");
        //         assert(offset_uloc > -1);
        //         glUniform2fv(offset_uloc, 1, (float *)&particle.position);

        //         //this->shader.SetVector4f("color", particle.Color);
        //         GLint color_uloc = glGetUniformLocation(shaderProgram, "color");
        //         assert(color_uloc > -1);
        //         glUniform4fv(color_uloc, 1, (float *)&particle.color);

        //         //this->texture.Bind();
        //         glBindTexture(GL_TEXTURE_2D, (GLuint)used_texture);

        //         glBindVertexArray(vao);
        //         glDrawArrays(GL_TRIANGLES, 0, 6);
        //         glBindVertexArray(0);
        //     }
        // }        

        glBindTexture(GL_TEXTURE_2D, used_texture);
        glBindVertexArray(vao);

        // Bind instanced VBO again to update values of particles
        glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particles), particles);

        // Instanced rendering call
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, max_particles);

        // Clean up
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        // don't forget to reset to default blending mode
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void ParticleGenerator::init()
{
    // set up mesh and attribute properties
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    }; 
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &VBO);
    glBindVertexArray(vao);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // glBindVertexArray(0);

    // 
    for (int i = 0; i < max_particles; i++) {
        particles[i] = Particle();
    }
    initialized_entity_id = blocks[0];
    for (int i = 0; i < MAX_PARTICLE_ENTITIES; i++) {
        blocks[i] = initialized_entity_id;
    }

    // Generate instance VBO
    glGenBuffers(1, &instance_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STATIC_DRAW);

    // Point aOffset attribute to each Particle's position in particles array
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glVertexAttribDivisor(1, 1); // attribute at layout 1 is instanced

    // Point aColor attribute to each Particle's color in particles array
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(vec2)));
    glVertexAttribDivisor(2, 1); // attribute at layout 2 is instanced

    // Point aScale attribute to each Particle's scale in particles array
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(vec2) + sizeof(vec4) + sizeof(float)));
    glVertexAttribDivisor(3, 1); // attribute at layout 3 is instanced

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int ParticleGenerator::firstUnusedParticle(int lastUsedParticle, int begin, int end)
{
    // first search from last used particle, this will usually return almost instantly
    for (int i = lastUsedParticle; i < end; ++i){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (int i = begin; i < lastUsedParticle; ++i){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, Entity entity, glm::vec2 offset)
{
    std::cout << "WARNING: Base class ParticleGenerator::respawnParticle has been called" << "\n";
    return; // should be overriden in subclasses 
}

int ParticleGenerator::findUnusedBlock()
{
    // Find an available block index and return it
    for (int i = 0; i < MAX_PARTICLE_ENTITIES; i++) {
        // 0 when initialized, or free when previous Entity is no longer registered for particles
        if (blocks[i] == 0 || !registry.particleEffects.has(blocks[i])) {
            std::cout << "New block index:" << i << "\n";
            return i;
        }
    }
    return -1;
}

PARTICLE_TYPE_ID ParticleGenerator::getType()
{
    std::cout << "WARNING: Base class ParticleGenerator::getType has been called" << "\n";
    return PARTICLE_TYPE_ID::PARTICLE_TYPE_COUNT; // should be overriden in subclasses 
}