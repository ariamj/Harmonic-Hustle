// internal
#include "ai_system.hpp"

const float CHASE_PLAYER_RADIUS = 300.f;

void AISystem::step(float elapsed_ms)
{
	// iterate over enemies
	// if player is within certain radius
	//		chase enemy
	// else
	//		find a direction they can move without hitting obstacle
	//		update enemy velocity

	auto& enemies = registry.enemies.entities;
	Motion& playerMotion = registry.motions.get(gameInfo.player_sprite);
	vec2 playerPos = playerMotion.position;
	for (Entity enemy : enemies) {
		Motion& enemyMotion = registry.motions.get(enemy);
		vec2 enemyPos = enemyMotion.position;
		float xDis = playerPos.x - enemyPos.x;
		float yDis = playerPos.y - enemyPos.y;
		float distance = xDis * xDis + yDis * yDis;
		distance = sqrt(distance);
		float enemyVelocity = 100.f;
		if (distance <= CHASE_PLAYER_RADIUS) {
			if (!registry.isChasing.has(enemy)) {
				registry.isChasing.emplace(enemy);
			};
			// printf("testing xdis, %f, y dis %f", xDis, yDis);
			float xPercent = xDis / (abs(xDis) + abs(yDis));
			float yPercent = yDis / (abs(xDis) + abs(yDis));
			enemyMotion.velocity = {enemyVelocity * xPercent, enemyVelocity * yPercent};
		} else {
			if (registry.isChasing.has(enemy))
				registry.isChasing.remove(enemy);
			enemyMotion.velocity = {0,0};
		}
	}

	(void)elapsed_ms; // placeholder to silence unused warning until implemented
}