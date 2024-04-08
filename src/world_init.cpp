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
	registry.screens.insert(entity, Screen::OVERWORLD);

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
	registry.screens.insert(entity, Screen::OVERWORLD);

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
		default:
			textureAssetId = TEXTURE_ASSET_ID::ENEMY_MIC;
			
	}

	// enemy level
	registry.levels.insert(entity, {level});

	// enemy battle info
	registry.battleProfiles.insert(entity, { 100.f * (float)level });

	// Create component
	registry.enemies.emplace(entity);
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

Entity createNote(RenderSystem* renderer, vec2 pos, float spawn_time, float duration) {
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 150.f };
	motion.position = pos;
	motion.scale = vec2({ NOTE_WIDTH, NOTE_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, Screen::BATTLE);
	
	// render in foreground
	registry.foregrounds.emplace(entity);

	// Create component
	Note& note = registry.notes.emplace(entity);
	// For interpolation based on absolute song time
	note.spawn_time = spawn_time;
	note.duration = duration;
	note.curr_duration = duration;

	registry.colours.insert(entity, { 1.f, 1.f, 1.f });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::NOTE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	// Give particles to entity
	ParticleEffect& particles = registry.particleEffects.emplace(entity);
	if (duration > 0.f) {
		particles.type = PARTICLE_TYPE_ID::TRAIL;
	}
	else {
		particles.type = PARTICLE_TYPE_ID::FLAME;
	}

	return entity;
}

Entity createNoteDuration(RenderSystem* renderer, vec2 pos, float duration) {
	auto entity = Entity();

	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 150.f };
	motion.position = pos;
	motion.scale = vec2({ NOTE_WIDTH, NOTE_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, Screen::BATTLE);

	// render in foreground
	registry.foregrounds.emplace(entity);

	NoteDuration& note_duration = registry.noteDurations.emplace(entity);
	note_duration.count_ms = duration;

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
	motion.velocity = { 0, 0 };
	motion.position = pos;
	motion.scale = vec2({ PORTRAIT_WIDTH, PORTRAIT_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, Screen::BATTLE);

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
	motion.velocity = { 0, 0 };
	motion.position = pos;
	motion.scale = vec2({ PORTRAIT_WIDTH, PORTRAIT_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, Screen::BATTLE);

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

Entity createCSTextbox(RenderSystem* renderer, vec2 pos)
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
	motion.scale = vec2({ 1700.f * gameInfo.width / K_MONITOR_WIDTH, 480 * gameInfo.height / K_MONITOR_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, Screen::CUTSCENE);

	// Create component
	// registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::BOX_CS,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}

Entity createCSEnemy(RenderSystem* renderer, vec2 pos, Screen screen)
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
	motion.scale = vec2({ CS_WIDTH * gameInfo.width / ORIGINAL_MONITOR_WIDTH, 
						CS_HEIGHT * gameInfo.height / ORIGINAL_MONITOR_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, screen);

	// Create component
	// registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::BOSS_CS,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}

Entity createCSPlayer(RenderSystem* renderer, vec2 pos, Screen screen)
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
	motion.scale = vec2({ CS_WIDTH * gameInfo.width / ORIGINAL_MONITOR_WIDTH, 
						CS_HEIGHT * gameInfo.height / ORIGINAL_MONITOR_HEIGHT });

	// screen entity exists in
	registry.screens.insert(entity, screen);

	// Create component
	// registry.enemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::PLAYER_CS,
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
	registry.screens.insert(entity, Screen::BATTLE);

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
	registry.screens.insert(entity, screen);
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

Entity createText(const std::string text, vec2 pos, float scale, vec3 colour, Screen screen, bool center_pos, bool static_text, glm::mat4 trans) {
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

	// non-changing or changing text
	if (static_text) {
		registry.staticTexts.emplace(entity);
	} else {
		registry.dynamicTexts.emplace(entity);
	}

	return entity;
}

Entity createButton(const std::string text, vec2 pos, float text_scale, vec2 size, vec3 text_colour, vec3 box_colour, Screen screen) {
	Entity btn_base = createBox(pos, size);

    registry.screens.insert(btn_base, screen);
	registry.foregrounds.emplace(btn_base);
    registry.colours.insert(btn_base, box_colour);
	BoxAreaBound& bound = registry.boxAreaBounds.emplace(btn_base);
	bound.left = pos.x - (size.x/2.f);
	bound.right = pos.x + (size.x/2.f);
	bound.top = pos.y - (size.y/2.f);
	bound.bottom = pos.y + (size.y/2.f);

	BoxButton& btn = registry.boxButtons.emplace(btn_base);
	btn.button_base = btn_base;
	btn.text = text;
	btn.text_scale = text_scale;
	btn.text_colour = text_colour;

	return btn_base;
}

Entity createSmoke(vec2 pos) {
	Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = {0.f, -50.f};

	// Add timer to remove entity automatically
	registry.particleTimers.emplace(entity);

	// Give trail particles to entity
	ParticleEffect& particles = registry.particleEffects.emplace(entity);
	particles.type = PARTICLE_TYPE_ID::SMOKE;
	particles.max_particles = 20;

	return entity;
}

Entity createSpark(vec2 pos, float max_duration, Entity entity_to_observe) {
	Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.velocity = { 0.f, -100.f };

	// Add timer to remove entity automatically
	ParticleTimer& timer = registry.particleTimers.emplace(entity);
	timer.count_ms = max_duration + 200.f;
	timer.entity_to_observe = entity_to_observe;

	// Give trail particles to entity
	ParticleEffect& particles = registry.particleEffects.emplace(entity);
	particles.type = PARTICLE_TYPE_ID::SPARK;
	// particles.max_particles = 20;

	return entity;
}

Entity createHelpImage(RenderSystem* renderer, vec2 pos, vec2 scale, TEXTURE_ASSET_ID helpImgId, Screen screen) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);
	
	// Setting initial motion values
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = pos;
	motion.scale = scale;

	// screen entity exists in
	registry.screens.insert(entity, screen);

	// Create component
	registry.renderRequests.insert(
		entity,
		{
			helpImgId,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE,
		}
	);

	return entity;
}