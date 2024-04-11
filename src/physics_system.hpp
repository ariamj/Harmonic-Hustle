#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "render_system.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms, RenderSystem* renderSystem);
	void check_collisions_between_notes_and_judgments(float elapsed_ms, RenderSystem* renderSystem);
	void check_collisions_between_player_and_enemies(float elapsed_ms, RenderSystem* renderSystem);
	void check_collisions_between_all_motions(float elapsed_ms, RenderSystem* renderSystem);
	void updateParticles(RenderSystem* renderSystem, float elapsed_ms);

	PhysicsSystem()
	{
	}
};