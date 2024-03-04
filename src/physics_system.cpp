// internal
#include "physics_system.hpp"
#include "world_init.hpp"

#include <iostream>

float walk_cycle = 0.f;
const float WALK_CYCLE_SPEED = 0.15;

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

 void handleWalkAnimation() {
	 Entity e = registry.players.entities[0];
	 RenderRequest& r = registry.renderRequests.get(e);

	 walk_cycle += WALK_CYCLE_SPEED;

	 if (walk_cycle <= 1.f) {
		 r.used_texture = TEXTURE_ASSET_ID::PLAYER_WALK_F1;
	 }
	 else if (walk_cycle > 1.f && walk_cycle <= 2.f) {
		 r.used_texture = TEXTURE_ASSET_ID::PLAYER_WALK_F2;
	 }
	 else if (walk_cycle > 2.f && walk_cycle <= 3.f) {
		 r.used_texture = TEXTURE_ASSET_ID::PLAYER_WALK_F3;
	 }
	 else {
		 walk_cycle = 0.f;
	 }
 }

void PhysicsSystem::step(float elapsed_ms, RenderSystem* renderSystem)
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

		// move player
		 if (registry.players.has(entity)) {
			 if (motion.velocity[0] != 0 || motion.velocity[1] != 0) handleWalkAnimation();

			 if ((motion.velocity[0] < 0 && motion.position[0] > PLAYER_HEIGHT / 2.f) || (motion.velocity[0] > 0 && motion.position[0] < (gameInfo.width - PLAYER_WIDTH / 2.f))) {
				motion.position[0] = motion.position[0] + (step_seconds * motion.velocity[0]);
			 }
			 
			 if ((motion.velocity[1] < 0 && motion.position[1] > PLAYER_HEIGHT / 2.f) || (motion.velocity[1] > 0 && motion.position[1] < (gameInfo.height - PLAYER_HEIGHT / 2.f))) {
				motion.position[1] = motion.position[1] + (step_seconds * motion.velocity[1]);
			 }
		 }
		 // move enemies
		if (registry.enemies.has(entity) && !registry.pauseEnemyTimers.has(*gameInfo.player_sprite)) {
			
			float xChange = step_seconds * motion.velocity.x;
			float yChange = step_seconds * motion.velocity.y;
			float newX = motion.position.x + xChange;
			float newY = motion.position.y + yChange;


			float height = abs(motion.scale.y);
			float width = abs(motion.scale.x);
			vec2 top = {motion.position.x, motion.position.y - height / 2.f};
			vec2 bot = {motion.position.x, motion.position.y + height / 2.f};
			vec2 left = {motion.position.x - width / 2.f, motion.position.y};
			vec2 right = {motion.position.x + width / 2.f, motion.position.y};

			// if enemy is running, 
			//		if not hit wall, keep running
			//		else, keep running along wall
			// if enemy is chasing or roaming
			//		if hit wall, bounce off
			// 		else update pos as normal

			if (registry.isRunning.has(entity)) {
				if (left.x + xChange >= 0 && right.x + xChange <= gameInfo.width && top.y + yChange >= 0 && bot.y + yChange <= gameInfo.height) {
					motion.position = {newX, newY};
				} else if (left.x + xChange >= 0 && right.x + xChange <= gameInfo.width ) {
					motion.position = {newX, motion.position.y};
				} else if (top.y + yChange >= 0 && bot.y + yChange <= gameInfo.height) {
					motion.position = {motion.position.x, newY};
				}
			} else {
				// hit top or bottom, change y
				// hit right/left change x
				if (right.x + xChange > gameInfo.width || left.x + xChange < 0) {
					motion.velocity.x = -1.f * motion.velocity.x;
				} else if (bot.y + yChange > gameInfo.height || top.y + yChange < 0) {
					motion.velocity.y = -1.f * motion.velocity.y;
				} else {
					motion.position = {newX, newY};
				}
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
			 Motion& motion = motion_registry.components[i];
			 Entity& entity = motion_registry.entities[i];
			 float line_thickness = 3.f;
			 if (registry.enemies.has(entity) || registry.players.has(entity)) {
				// create a dot in the motion.positin for ai debugging
				Entity spriteDot = createDot(motion.position, vec2{8, 8});

				 vec2 bb = get_bounding_box(motion);
				 Entity line1 = createLine(motion.position + vec2(bb.x / 2, 0.0), vec2(line_thickness, bb.y));
				 Entity line2 = createLine(motion.position - vec2(bb.x / 2, 0.0), vec2(line_thickness, bb.y));
				 Entity line3 = createLine(motion.position + vec2(0.0, bb.y / 2), vec2(bb.x, line_thickness));
				 Entity line4 = createLine(motion.position - vec2(0.0, bb.y / 2), vec2(bb.x, line_thickness));
			 }
			 // Note center lines for scoring
			 if (registry.notes.has(entity)) {
				Entity center_line = createLine(motion.position, vec2(motion.scale.x, line_thickness), Screen::BATTLE);
			 }
			 // Judgement line center line for scoring
			 if (registry.judgmentLine.has(entity)) {
				Entity center_line = createLine(motion.position, vec2(motion.scale.x * 0.8f, line_thickness), Screen::BATTLE);
				vec2 bb = get_bounding_box(motion);
				JudgementLine judgement_line = registry.judgmentLine.get(entity);
				Entity top = createLine(motion.position - vec2(0.f, bb.y * judgement_line.actual_img_scale_factor), vec2(bb.x * 0.8f, line_thickness), Screen::BATTLE);
				Entity bottom = createLine(motion.position + vec2(0.f, bb.y * judgement_line.actual_img_scale_factor), vec2(bb.x * 0.8f, line_thickness), Screen::BATTLE);
			 }
			 //TODO: Player Mesh
			 else if (registry.players.has(entity)) {
				// player radius for enemy ai debugging
				Entity playerRadius = createCircleOutline(motion.position, PLAYER_ENEMY_RADIUS);

				 // visualize mesh
				 Transform transform;
				 transform.translate(motion.position);
				 transform.rotate(motion.angle);
				 transform.scale(motion.scale);
				 mat3 proj_mat = renderSystem->createProjectionMatrix();
				 Mesh& mesh = *(registry.meshPtrs.get(entity));
				 float left_vertex_bound = 1, right_vertex_bound = -1, top_vertex_bound = -1, bot_vertex_bound = 1;
				 for (const ColoredVertex& v : mesh.vertices) {
					 // draw out the points of the mesh boundary
					 glm::vec3 vertex_trans = proj_mat * transform.mat * vec3({ v.position.x, v.position.y, 1.0f });
					 float vertex_x = vertex_trans.x;
					 float vertex_y = vertex_trans.y;
					 Entity vertex = createLine(vec2({ ((vertex_x + 1) / 2.f) * gameInfo.width, (1 - ((vertex_y + 1) / 2.f)) * gameInfo.height }), vec2({ motion.scale.x / 25, motion.scale.x / 25 }));
				 }
			 }
		 }
	 }
}