// internal
#include "ai_system.hpp"
#include <random>

void AISystem::step(float elapsed_ms)
{
	// iterate over enemies
	// if player is within certain radius
	//		chase enemy
	// else
	//		find a direction they can move without hitting obstacle
	//		update enemy velocity

	if (!registry.pauseEnemyTimers.has(*gameInfo.player_sprite)) {
		auto& enemies = registry.enemies.entities;
		Motion& playerMotion = registry.motions.get(*gameInfo.player_sprite);
		vec2 playerPos = playerMotion.position;
		int playerLevel = registry.levels.get(*gameInfo.player_sprite).level;
		for (Entity enemy : enemies) {
			Motion& enemyMotion = registry.motions.get(enemy);
			vec2 enemyPos = enemyMotion.position;
			float xDis = playerPos.x - enemyPos.x;
			float yDis = playerPos.y - enemyPos.y;
			float distance = xDis * xDis + yDis * yDis;
			distance = sqrt(distance);

			int enemyLevel = registry.levels.get(enemy).level;

			// chase if higher level, else run away
			if (enemyLevel > playerLevel) {
				if (distance <= PLAYER_ENEMY_RADIUS) {
					if (!registry.isChasing.has(enemy)) {
						registry.isChasing.emplace(enemy);
					};
					// printf("testing xdis, %f, y dis %f", xDis, yDis);
					float xPercent = xDis / (abs(xDis) + abs(yDis));
					float yPercent = yDis / (abs(xDis) + abs(yDis));
					enemyMotion.velocity = {CHASE_TOTAL_VELOCITY * xPercent, CHASE_TOTAL_VELOCITY * yPercent};
				} else {
					if (registry.isChasing.has(enemy)) {
						registry.isChasing.remove(enemy);
						enemyMotion.velocity = getRandomEnemyVelocity();
					}
				}
			} else {
				if (distance <= PLAYER_ENEMY_RADIUS) {
					if (!registry.isRunning.has(enemy)) {
						registry.isRunning.emplace(enemy);
					};
					// printf("testing xdis, %f, y dis %f", xDis, yDis);
					float xPercent = xDis / (abs(xDis) + abs(yDis));
					float yPercent = yDis / (abs(xDis) + abs(yDis));
					enemyMotion.velocity = {RUN_AWAY_TOTAL_VELOCITY * xPercent * -1.f, RUN_AWAY_TOTAL_VELOCITY * yPercent * -1.f};
				} else {
					if (registry.isRunning.has(enemy)) {
						registry.isRunning.remove(enemy);
						enemyMotion.velocity = getRandomEnemyVelocity();
					}
				}
			}
		}
	} else {
		// even if they're paused, add appropriate isChasing / isRunning component to them since player moves
		auto& enemies = registry.enemies.entities;
		Motion& playerMotion = registry.motions.get(*gameInfo.player_sprite);
		vec2 playerPos = playerMotion.position;
		int playerLevel = registry.levels.get(*gameInfo.player_sprite).level;
		for (Entity enemy : enemies) {
			Motion& enemyMotion = registry.motions.get(enemy);
			vec2 enemyPos = enemyMotion.position;
			float xDis = playerPos.x - enemyPos.x;
			float yDis = playerPos.y - enemyPos.y;
			float distance = xDis * xDis + yDis * yDis;
			distance = sqrt(distance);

			int enemyLevel = registry.levels.get(enemy).level;

			// chase if higher level, else run away
			if (enemyLevel > playerLevel) {
				if (distance <= PLAYER_ENEMY_RADIUS) {
					if (!registry.isChasing.has(enemy)) {
						registry.isChasing.emplace(enemy);
					};
				} else {
					if (registry.isChasing.has(enemy)) {
						registry.isChasing.remove(enemy);
					}
				}
			} else {
				if (distance <= PLAYER_ENEMY_RADIUS) {
					if (!registry.isRunning.has(enemy)) {
						registry.isRunning.emplace(enemy);
					};
				} else {
					if (registry.isRunning.has(enemy)) {
						registry.isRunning.remove(enemy);
					}
				}
			}
		}
	}
}