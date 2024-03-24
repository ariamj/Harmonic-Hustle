// Adapted from LearnOpenGL:
// https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/3.2d_game/0.full_source/particle_generator.h

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>
#include <map>

// Commented out includes from LearnOpenGL
// #include <glad/glad.h>
// #include "shader.h"
// #include "texture.h"
// #include "game_object.h"


// #include <glm/glm.hpp>
#include "../ext/glm/glm/glm.hpp"

// Represents a single particle and its state
struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;
    float     life;
    glm::vec2 scale;

    Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f), scale(DEFAULT_PARTICLE_SCALE) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(GLuint shaderProgram, GLuint used_texture);
    // update all particles
    void Update(float dt, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // render all particles
    void Draw();

private:
    // render state
    GLuint shaderProgram;
    GLuint used_texture;
    GLuint vao;
    GLuint instance_VBO;

    void init();
    void updateEntities();

protected: 
    // state
    static const int amount = 500; // hard-coded for now.
    static const int max_particles = amount * MAX_PARTICLE_ENTITIES;
    Entity blocks[MAX_PARTICLE_ENTITIES];
    Entity initialized_entity_id;
    Particle particles[max_particles];
    int findUnusedBlock();
    unsigned int firstUnusedParticle(int lastUsedParticle, int begin, int end);

    // updates particles to achieve desired effects
    virtual void updateParticleBehaviours(Particle* p, float dt);
    // respawns particle
    virtual void respawnParticle(Particle& particle, Entity entity, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif

