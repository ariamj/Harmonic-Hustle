// internal
#include "physics_system.hpp"
#include "world_init.hpp"

#include "chrono"
using Clock = std::chrono::high_resolution_clock;

#include <iostream>
#include <map>

std::vector<std::pair<uint16_t, uint16_t>> boundary_edges;
std::set<uint16_t> boundary_indices;

// get the edges that make up the mesh boundary
void fill_mesh_boundary_edge(Mesh& mesh) {
	std::vector<std::vector<uint16_t>> faces;
	std::map<std::pair<uint16_t, uint16_t>, int> edges;
	//std::vector<std::pair<uint16_t, uint16_t>> boundary_edges;

	for (int i = 0; i < mesh.vertex_indices.size(); i += 3) {
		std::vector<uint16_t> face;
		face.push_back(mesh.vertex_indices[i]);
		face.push_back(mesh.vertex_indices[i + 1]);
		face.push_back(mesh.vertex_indices[i + 2]);
		faces.push_back(face);
	}

	// store the edges (indices):
	for (auto face : faces) {
		// <v1, v2> where v1 > v2
		std::pair<uint16_t, uint16_t> edge1(std::max(face[0], face[1]), std::min(face[0], face[1]));
		std::pair<uint16_t, uint16_t> edge2(std::max(face[0], face[2]), std::min(face[0], face[2]));
		std::pair<uint16_t, uint16_t> edge3(std::max(face[1], face[2]), std::min(face[1], face[2]));

		edges[edge1] += 1;
		edges[edge2] += 1;
		edges[edge3] += 1;
	}

	// go thru all edges and add the boundary edges only (boundary edges only referenced by one face, so edge count == 1)
	for (auto keyval : edges) {
		if (keyval.second == 1) {
			auto edge = keyval.first;
			boundary_edges.push_back(edge);
			boundary_indices.insert(edge.first);
			boundary_indices.insert(edge.second);
		}
	}
	printf("number of total edges = %zu\n", edges.size());
	printf("number of boundary edges = %zu\n", boundary_edges.size());
}

//https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
// Given three collinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool onSegment(vec2 p, vec2 q, vec2 r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}

//https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are collinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(vec2 p, vec2 q, vec2 r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // collinear 

	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

//https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool doIntersect(vec2 p1, vec2 q1, vec2 p2, vec2 q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are collinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are collinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are collinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are collinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}


// mesh = player, motion 2 = npc
// check for boundary edge intersection of player with edges of the AABB of npc (just mesh-line for now)
bool collides_mesh_line(Mesh& mesh, const Motion& motion, const Motion& motion2) {
	(void)motion2;
	float displacement = 30.f;
	float center2x = motion2.position.x;
	float center2y = motion2.position.y;
	float left2 = center2x - displacement;
	float right2 = center2x + displacement;
	float top2 = center2y - displacement;
	float bottom2 = center2y + displacement;
	//auto upper_right_point = vec2(right2, top2);
	//auto upper_left_point = vec2(left2, top2);
	//auto lower_right_point = vec2(right2, bottom2);
	//auto lower_left_point = vec2(left2, bottom2);

	// Define AABB edges of NPC
	vec2 topLeft(left2, top2);
	vec2 topRight(right2, top2);
	vec2 botRight(right2, bottom2);
	vec2 botLeft(left2, bottom2);

	for (auto edge : boundary_edges) {
		// get world coords of the edge vertices:
		auto& v1 = mesh.vertices[edge.first];
		auto& v2 = mesh.vertices[edge.second];
		Transform transform;
		transform.translate(motion.position);
		transform.rotate(motion.angle);
		transform.scale(motion.scale);

		// world coords
		auto world_v1 = transform.mat * vec3(v1.position.x, v1.position.y, 1.0f);
		// float v1_x = world_v1.x;
		// float v1_y = world_v1.y;
		auto world_v2 = transform.mat * vec3(v2.position.x, v2.position.y, 1.0f);
		// float v2_x = world_v2.x;
		// float v2_y = world_v2.y;
		// float m_12 = (v2_y - v1_y) / (v2_x - v1_x);

		vec2 p1(world_v1.x, world_v1.y);
		vec2 q1(world_v2.x, world_v2.y);

		// Check for intersection with AABB edges
		if (doIntersect(p1, q1, topLeft, topRight) ||
			doIntersect(p1, q1, topRight, botRight) ||
			doIntersect(p1, q1, botRight, botLeft) ||
			doIntersect(p1, q1, botLeft, topLeft)) {
			return true;
		}
	}
	return false;
}

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

	// for allowing notes to be collided with slightly more above/below lines
	float extra_vertical_multiplier = 1.1f;

	float center1y = motion1.position.y;
	float top1 = center1y - displacement * extra_vertical_multiplier;
	float bottom1 = center1y +  displacement * extra_vertical_multiplier;

	float center2y = motion2.position.y;
	float top2 = center2y - displacement * extra_vertical_multiplier;
	float bottom2 = center2y + displacement * extra_vertical_multiplier;

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
	// Clear all collisions from previous step
	registry.collisions.clear();

	 // Move entities
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++) {
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		float step_seconds = elapsed_ms / 1000.f;

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

	// auto t1 = Clock::now();

	// Check for specific collisions 
	switch (gameInfo.curr_screen) {
		case OVERWORLD:
			check_collisions_between_player_and_enemies(elapsed_ms, renderSystem);
			break;
		case BATTLE:
			check_collisions_between_notes_and_judgments(elapsed_ms, renderSystem);
			break;
		default:
			break;
	}
	// check_collisions_between_all_motions(elapsed_ms, renderSystem);

	// auto t2 = Clock::now();
	// float collisions_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)).count() / 1000;
	// std::cout << "Checking collisions time:" << collisions_ms << "\n";

	updateParticles(renderSystem, elapsed_ms);

	if (debugging.in_debug_mode) {
		for (int i = 0; i < motion_registry.components.size(); ++i) {
			Motion& motion = motion_registry.components[i];
			Entity& entity = motion_registry.entities[i];
			float line_thickness = 3.f;
			if (registry.enemies.has(entity) || registry.players.has(entity)) {
			// create a dot in the motion.positin for ai debugging
			createDot(motion.position, vec2{8, 8});

				vec2 bb = get_bounding_box(motion);
				createLine(motion.position + vec2(bb.x / 2, 0.0), vec2(line_thickness, bb.y)); //line1
				createLine(motion.position - vec2(bb.x / 2, 0.0), vec2(line_thickness, bb.y)); //line2
				createLine(motion.position + vec2(0.0, bb.y / 2), vec2(bb.x, line_thickness)); //line3
				createLine(motion.position - vec2(0.0, bb.y / 2), vec2(bb.x, line_thickness)); //line4
			}
			// Note center lines for scoring
			if (registry.notes.has(entity)) {
			createLine(motion.position, vec2(motion.scale.x, line_thickness), Screen::BATTLE);
			}
			// Judgement line center line for scoring
			if (registry.judgmentLine.has(entity)) {
			// center line
			createLine(motion.position, vec2(motion.scale.x * 0.8f, line_thickness), Screen::BATTLE);
			vec2 bb = get_bounding_box(motion);
			JudgementLine judgement_line = registry.judgmentLine.get(entity);
			// top
			createLine(motion.position - vec2(0.f, bb.y * judgement_line.actual_img_scale_factor), vec2(bb.x * 0.8f, line_thickness), Screen::BATTLE);
			// bottom
			createLine(motion.position + vec2(0.f, bb.y * judgement_line.actual_img_scale_factor), vec2(bb.x * 0.8f, line_thickness), Screen::BATTLE);
			}

			// mesh
			if (registry.players.has(entity)) {

			// player radius for enemy ai debugging
			createCircleOutline(motion.position, PLAYER_ENEMY_RADIUS);

				// visualize mesh
				Transform transform;
				transform.translate(motion.position);
				transform.rotate(motion.angle);
				transform.scale(motion.scale);
				mat3 projection = renderSystem->createProjectionMatrix();
				Mesh& mesh = *(registry.meshPtrs.get(entity));
			//  float left_vertex_bound = 1, right_vertex_bound = -1, top_vertex_bound = -1, bot_vertex_bound = 1;
				// visualize the boundary vertices of the mesh
				for (auto& index : boundary_indices) {
					auto& v = mesh.vertices[index];
					auto world_v = projection * transform.mat * vec3(v.position.x, v.position.y, 1.f);
					// vertex
					createLine(vec2({ ((world_v.x + 1) / 2.f) * gameInfo.width, (1 - ((world_v.y + 1) / 2.f)) * gameInfo.height }),
						vec2({ motion.scale.x / 30, motion.scale.x / 30 }));
				}
			}
		}
	}
}

// Check for collisions sbetween player and enemies
void PhysicsSystem::check_collisions_between_player_and_enemies(float elapsed_ms, RenderSystem* renderSystem) {
	Entity player = registry.players.entities[0];
	Motion& player_motion = registry.motions.get(player);
	for (auto enemy : registry.enemies.entities) {
		Motion& enemy_motion = registry.motions.get(enemy);
		Mesh* m_i = registry.meshPtrs.get(player);

		if (boundary_indices.size() < 1) {
			fill_mesh_boundary_edge(*m_i);
		}

		// do mesh-line collision
		if (collides_mesh_line(*m_i, player_motion, enemy_motion)) {
			registry.collisions.emplace_with_duplicates(player, enemy);
			registry.collisions.emplace_with_duplicates(enemy, player);
		}
	}
}

// Check for collisions between judgment lines and notes
void PhysicsSystem::check_collisions_between_notes_and_judgments(float elapsed_ms, RenderSystem* renderSystem) {
	for (auto judgment : registry.judgmentLine.entities) {
		Motion& judgment_motion = registry.motions.get(judgment);
		for (auto note : registry.notes.entities) {
			Motion& note_motion = registry.motions.get(note);
			if (collides(judgment_motion, note_motion))
			{
				registry.collisions.emplace_with_duplicates(judgment, note);
				registry.collisions.emplace_with_duplicates(note, judgment);
			}
		}
	}
}

// Original implementation
void PhysicsSystem::check_collisions_between_all_motions(float elapsed_ms, RenderSystem* renderSystem) {
	auto& motion_registry = registry.motions;	
	 // Check for collisions between all moving entities
	 for (uint i = 0; i < motion_registry.components.size(); i++)
	 {
		 Motion& motion_i = motion_registry.components[i];
		 Entity entity_i = motion_registry.entities[i];

		 // note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		 for (uint j = i + 1; j < motion_registry.components.size(); j++)
		 {
			 Motion& motion_j = motion_registry.components[j];
			 Entity entity_j = motion_registry.entities[j];
			 if (registry.players.has(entity_i)) {
				 Mesh* m_i = registry.meshPtrs.get(entity_i);

				 if (boundary_indices.size() < 1) {
					 fill_mesh_boundary_edge(*m_i);
				 }

				 // do mesh-line collision
				 if (collides_mesh_line(*m_i, motion_i, motion_j)) {
					 // Create a collisions event
				 // We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
					 registry.collisions.emplace_with_duplicates(entity_i, entity_j);
					 registry.collisions.emplace_with_duplicates(entity_j, entity_i);
				 }
			 }
			 else if (registry.players.has(entity_j)) {
				 Mesh* m_j = registry.meshPtrs.get(entity_j);

				 if (boundary_indices.size() < 1) {
					 fill_mesh_boundary_edge(*m_j);
				 }

				 if (collides_mesh_line(*m_j, motion_j, motion_i)) {

					 // Create a collisions event
					 // We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
					 registry.collisions.emplace_with_duplicates(entity_i, entity_j);
					 registry.collisions.emplace_with_duplicates(entity_j, entity_i);
				 }
			 }

			 else {
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
	 }
}

void PhysicsSystem::updateParticles(RenderSystem* renderSystem, float elapsed_ms) {
	// Update particles
	int new_particles = 2;
	float dt = elapsed_ms / 1000.f;
	for (auto generator : renderSystem->particle_generators) {
		// Generate particles of all types currently allocated
		if (generator != NULL) {
			generator->Update(dt, new_particles, vec2(0.f, 0.f));
		}
	}
	// Step timers for timed particle entities
	for (auto entity : registry.particleTimers.entities) {
		ParticleTimer& timer = registry.particleTimers.get(entity);
		timer.count_ms -= elapsed_ms;
		if (timer.count_ms <= 0.f) {
			registry.remove_all_components_of(entity);
		}
	}
}