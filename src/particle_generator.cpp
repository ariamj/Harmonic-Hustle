

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

ParticleGenerator::ParticleGenerator(GLuint shaderProgram, TEXTURE_ASSET_ID used_texture, unsigned int amount, Entity entity)
    : entity(entity), amount(amount), shaderProgram(shaderProgram), used_texture(used_texture)
{
    init();
}

void ParticleGenerator::Update(float dt, unsigned int newParticles, glm::vec2 offset)
{
    // add new particles 
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle], offset);
    }
    // update all particles
    for (unsigned int i = 0; i < amount; ++i)
    {
        Particle* p = &particles[i];
        p->life -= dt; // reduce life
        if (p->life > 0.0f)
        {	// particle is alive, thus update
            p->position += p->velocity * dt; 
            p->color.a -= dt * 2.5;
        }
    }
}

// render all particles
void ParticleGenerator::Draw()
{
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

    glBindTexture(GL_TEXTURE_2D, (GLuint)used_texture);
    glBindVertexArray(vao);

    // Bind instanced VBO again to update values of particles
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STATIC_DRAW);

    // Instanced rendering call
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 500); // 500 triangles of 6 vertices each

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    // create this->amount default particle instances
    for (unsigned int i = 0; i < amount; ++i)
        particles[i] = Particle();

    // Generate instance VBO
    glGenBuffers(1, &instance_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);

    // Point aOffset attribute to each Particle's position in particles array
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glVertexAttribDivisor(1, 1); // attribute at layout 1 is instanced

    // Point aColor attribute to each Particle's color in particles array
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(vec2)));
    glVertexAttribDivisor(2, 1); // attribute at layout 2 is instanced

    // Point aLife attribute to each Particle's life in particles array
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(vec2) + sizeof(vec4)));
    glVertexAttribDivisor(3, 1); // attribute at layout 2 is instanced

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < amount; ++i){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    Motion& entity_motion = registry.motions.get(entity);
    particle.position = entity_motion.position + random + offset;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.life = 1.f;
    particle.velocity = entity_motion.velocity * 0.1f;
}

