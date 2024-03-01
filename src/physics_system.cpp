// internal
#include "physics_system.hpp"
#include "world_init.hpp"

#include <iostream>

// Returns the local bounding coordinates scaled by the current size of the entity
 vec2 get_bounding_box(const Motion& motion)
 {
 	// abs is to avoid negative scale due to the facing direction.
	// TODO: Keep region constant while increasing scale of note? (smaller -> bigger from top to bottom of screen)
		// This current implementation will increase collision region as note gets bigger
 	return { abs(motion.scale.x), abs(motion.scale.y) };
 }

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
 bool collides(const Motion& motion1, const Motion& motion2)
 {
	float center1x = motion1.position.x;
	float displacement = 30.f;
	float left1 = center1x - displacement;
	float right1 = center1x + displacement;

	float center2x = motion2.position.x;
	float left2 = center2x - displacement;
	float right2 = center2x + displacement;

	float center1y = motion1.position.y;
	float top1 = center1y - displacement;
	float bottom1 = center1y + displacement;

	float center2y = motion2.position.y;
	float top2 = center2y - displacement;
	float bottom2 = center2y + displacement;

	// horizontal: top1 < bottom2 && top2 < bottom1
	if (top1 < bottom2 && top2 < bottom1) {
		// vertical: left1 < right2 & left2 < right1
		if (left1 < right2 && left2 < right1) {
			return true;
		}
	}

	return false;

 	// vec2 dp = motion1.position - motion2.position;
 	// float dist_squared = dot(dp,dp);
 	// const vec2 other_bonding_box = get_bounding_box(motion1) / 10.f;
 	// const float other_r_squared = dot(other_bonding_box, other_bonding_box);
 	// const vec2 my_bonding_box = get_bounding_box(motion2) / 10.f;
 	// const float my_r_squared = dot(my_bonding_box, my_bonding_box);
 	// const float r_squared = max(other_r_squared, my_r_squared);
 	// if (dist_squared < r_squared)
 	// 	return true;

 	// return false;
 }

void PhysicsSystem::step(float elapsed_ms)
{
	 // Move entities
	 auto& motion_registry = registry.motions;
	 for(uint i = 0; i< motion_registry.size(); i++)
	 {
		 Motion& motion = motion_registry.components[i];
		 Entity entity = motion_registry.entities[i];

		 float step_seconds = elapsed_ms / 1000.f;

		 GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		 const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		 if (registry.players.has(entity)) {
			 if ((motion.velocity[0] < 0 && motion.position[0] > (0 + PLAYER_WIDTH/2)) || (motion.velocity[0] > 0 && motion.position[0] < (mode->width - PLAYER_WIDTH - 175))) {
				motion.position[0] = motion.position[0] + (step_seconds * motion.velocity[0]);
			 }
			 
			 if ((motion.velocity[1] < 0 && motion.position[1] > (0 + PLAYER_HEIGHT / 2)) || (motion.velocity[1] > 0 && motion.position[1] < (mode->height - PLAYER_HEIGHT))) {
				motion.position[1] = motion.position[1] + (step_seconds * motion.velocity[1]);
			 }
			 
		 }
	 }

	 // Check for collisions between all moving entities
	 for(uint i = 0; i< motion_registry.components.size(); i++)
	 {
	 	Motion& motion_i = motion_registry.components[i];
	 	Entity entity_i = motion_registry.entities[i];
		
	 	// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
	 	for(uint j = i+1; j< motion_registry.components.size(); j++)
	 	{
	 		Motion& motion_j = motion_registry.components[j];
	 		if (collides(motion_i, motion_j))
	 		{
				// std::cout << "COLLIDING" << std::endl;
	 			Entity entity_j = motion_registry.entities[j];
	 			// Create a collisions event
	 			// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
	 			registry.collisions.emplace_with_duplicates(entity_i, entity_j);
	 			registry.collisions.emplace_with_duplicates(entity_j, entity_i);
	 		}
	 	}
	 }
	 if (debugging.in_debug_mode) {
		 for (int i = 0; i < motion_registry.components.size(); ++i) {
			 Motion& motion = registry.motions.components[i];
			 Entity& entity = motion_registry.entities[i];
			 if (registry.enemies.has(entity)) {
				 vec2 bb = get_bounding_box(motion);
				 Entity line1 = createLine(motion.position + vec2(bb.x / 2, 0.0), vec2(3.0, bb.y));
				 Entity line2 = createLine(motion.position - vec2(bb.x / 2, 0.0), vec2(3.0, bb.y));
				 Entity line3 = createLine(motion.position + vec2(0.0, bb.y / 2), vec2(bb.x, 3.0));
				 Entity line4 = createLine(motion.position - vec2(0.0, bb.y / 2), vec2(bb.x, 3.0));
			 }
		 }
	 }
}