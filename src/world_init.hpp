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

// const float LANE_1 = gameInfo.width / 2 - 300;
// const float LANE_2 = gameInfo.width / 2 - 100;
// const float LANE_3 = gameInfo.width / 2 + 100;
// const float LANE_4 = gameInfo.width / 2 + 300;

// the player
Entity createPlayer(RenderSystem* renderer, vec2 pos);
// the enemy
Entity createEnemy(RenderSystem* renderer, vec2 pos, int level);
// player in battle scene
Entity createBattlePlayer(RenderSystem* renderer, vec2 pos);
// enemy in battle scene
Entity createBattleEnemy(RenderSystem* renderer, vec2 pos);
// judgement line
Entity createJudgementLine(RenderSystem* renderer, vec2 pos);
// notes
Entity createNote(RenderSystem* renderer, vec2 pos);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size, Screen screen = Screen::OVERWORLD);
// an empty white box
Entity createBox(vec2 position, vec2 size);
// a circle outline for radius debugging purposes
Entity createCircleOutline(vec2 pos, float radius);
// a filled in circle for debugging purposes
Entity createDot(vec2 pos, vec2 size);