#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture

// the player
Entity createPlayer(RenderSystem* renderer, vec2 pos);

Entity createEnemy(RenderSystem* renderer, vec2 pos);

