#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
// const float PLAYER_WIDTH = 0.6f * 165.f;
// const float PLAYER_HEIGHT = 0.6f * 165.f;
// const float ENEMY_WIDTH = 0.6f * 165.f;
// const float ENEMY_HEIGHT = 0.6f * 165.f;
// const float PORTRAIT_WIDTH = 3 * 165.f;
// const float PORTRAIT_HEIGHT = 3 * 165.f;

// the player
Entity createPlayer(RenderSystem* renderer, vec2 pos);
// the enemy
Entity createEnemy(RenderSystem* renderer, vec2 pos, int scene);

// player in battle scene
Entity createBattlePlayer(RenderSystem* renderer, vec2 pos);
// enemy in battle scene
Entity createBattleEnemy(RenderSystem* renderer, vec2 pos);
// judgement line
Entity createJudgementLine(RenderSystem* renderer, vec2 pos);
