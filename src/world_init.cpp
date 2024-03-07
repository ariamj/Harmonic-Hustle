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

	// player level always starts at 1
	registry.levels.insert(entity, {1});

	// Create component
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::PLAYER_WALK_F1,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 pos, int level)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = getRandomEnemyVelocity();
	motion.position = pos;
	motion.scale = vec2({ ENEMY_WIDTH, ENEMY_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, {Screen::OVERWORLD});

	TEXTURE_ASSET_ID textureAssetId = TEXTURE_ASSET_ID::ENEMY_GUITAR;

	switch (level) {
		case 1:
			// guitar
			textureAssetId = TEXTURE_ASSET_ID::ENEMY_GUITAR;
			break;
		case 2: 
			// drum
			textureAssetId = TEXTURE_ASSET_ID::ENEMY_DRUM;
			break;
		case 3:
			// mic
		defualt:
			textureAssetId = TEXTURE_ASSET_ID::ENEMY_MIC;
			
	}

	// enemy level
	registry.levels.insert(entity, {level});

	// enemy battle info
	registry.battleProfiles.insert(entity, { 200.f * (float)level });

	// Create component
	Enemy enemy = registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			textureAssetId,
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
	auto& judgement_line = registry.judgmentLine.emplace(entity);
	judgement_line.actual_img_scale_factor = 1.f/6.f;
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

Entity createLine(vec2 position, vec2 scale, Screen screen)
{
	Entity entity = Entity();
	// screen entity exists in
	registry.screens.insert(entity, { screen });
	// registry.screens.insert(entity, { Screen::OVERWORLD });
	//registry.screens.insert(entity, { Screen::BATTLE });
	 //registry.colours.insert(entity, {1.f, 1.f, 1.f});
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::PLAYER,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

// renders a white box (using to render battle result popup)
Entity createBox(vec2 position, vec2 scale) {
	Entity entity = Entity();
	// registry.screens.insert(entity, { Screen::BATTLE });
	registry.renderRequests.insert(entity, {
		TEXTURE_ASSET_ID::TEXTURE_COUNT,
		EFFECT_ASSET_ID::PLAYER,
		GEOMETRY_BUFFER_ID::BOX }
	);

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	return entity;
}

Entity createCircleOutline(vec2 pos, float radius) {
	Entity entity = Entity();

	// screen entity exists in
	registry.screens.insert(entity, Screen::OVERWORLD);

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::PLAYER,
		 GEOMETRY_BUFFER_ID::CIRCLE_OUTLINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = pos;
	motion.scale = {radius * 2.f, radius * 2.f};

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createDot(vec2 pos, vec2 size)
{
	auto entity = Entity();

	registry.screens.insert(entity, Screen::OVERWORLD);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	// Create and (empty) Chicken component to be able to refer to all eagles
	registry.debugComponents.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::PLAYER,
			GEOMETRY_BUFFER_ID::DOT });

	return entity;
}

Entity createText(const std::string text, vec2 pos, float scale, vec3 colour, glm::mat4 trans, Screen screen, bool center_pos) {
	Entity entity = Entity();

	// Create text component to be rendered
	registry.texts.insert(
		entity,
		{
			text,
			pos,
			scale,
			colour,
			trans,
			screen,
			center_pos,
		}
	);

	return entity;
}
