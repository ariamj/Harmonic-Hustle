#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createPlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = pos;
	motion.scale = vec2({ PLAYER_WIDTH, PLAYER_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, {Screen::OVERWORLD});

	// Create component
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::PLAYER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 50, 50 };
	motion.position = pos;
	motion.scale = vec2({ ENEMY_WIDTH, ENEMY_HEIGHT });

	// screen entity exists in
		registry.screens.insert(entity, {Screen::OVERWORLD});
	
	// Create component
	registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::ENEMY,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}

Entity createNote(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 150, 150 };
	motion.position = pos;
	motion.scale = vec2({ NOTE_WIDTH, NOTE_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, { Screen::BATTLE });

	// Create component
	registry.notes.emplace(entity);
	registry.colours.insert(entity, { 1.f, 1.f, 1.f });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::NOTE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;

}

Entity createBattlePlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 50, 50 };
	motion.position = pos;
	motion.scale = vec2({ PORTRAIT_WIDTH, PORTRAIT_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, {Screen::BATTLE});

	// Create component
	// registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::BATTLEPLAYER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}

Entity createBattleEnemy(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 50, 50 };
	motion.position = pos;
	motion.scale = vec2({ PORTRAIT_WIDTH, PORTRAIT_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, {Screen::BATTLE});

	// Create component
	// registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::BATTLEENEMY,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}

Entity createJudgementLine(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0,0 };
	motion.position = pos;
	motion.scale = vec2({ 200, 200 });

	// screen entity exists in
	registry.screens.insert(entity, { Screen::BATTLE });

	// Create component
	registry.judgmentLine.emplace(entity);
	registry.colours.insert(entity, {1.f, 1.f, 1.f});
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::JUDGEMENT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}
