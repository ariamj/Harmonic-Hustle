#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<IsChasing> isChasing;
	ComponentContainer<IsRunning> isRunning;
	ComponentContainer<Level> levels;
	ComponentContainer<BattleProfile> battleProfiles;
	ComponentContainer<PauseEnemyTimer> pauseEnemyTimers;
	ComponentContainer<BattleOverPopUp> battleOverPopUpParts;
	ComponentContainer<BattleReminderPopUp> battleReminderPopUpParts;
	ComponentContainer<TutorialParts> tutorialParts;
	ComponentContainer<Disabled> disabled;
	ComponentContainer<Foreground> foregrounds;
	ComponentContainer<Background> backgrounds;
	ComponentContainer<Motion> motions;
	ComponentContainer<Note> notes;
	ComponentContainer<NoteDuration> noteDurations;
	ComponentContainer<Player> players;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Conductor> conductors;
	ComponentContainer<Screen> screens;
	ComponentContainer<Collision> collisions;
	ComponentContainer<JudgementLine> judgmentLine;
	ComponentContainer<CollisionTimer> collisionTimers;
	ComponentContainer<JudgementLineTimer> judgmentLineTimers;
	ComponentContainer<ProgressBar> progressBars;
	ComponentContainer<vec3> colours;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<Text> texts;
	ComponentContainer<StaticText> staticTexts;
	ComponentContainer<DynamicText> dynamicTexts;
	ComponentContainer<TextTimer> textTimers;
	ComponentContainer<BoxButton> boxButtons;
	ComponentContainer<ParticleEffect> particleEffects;
	ComponentContainer<BoxAreaBound> boxAreaBounds;
	ComponentContainer<EnemyCS> enemyCS;
	ComponentContainer<PlayerCS> playerCS;
	ComponentContainer<CSText> CSTexts;
	ComponentContainer<CSTextbox> CSTextbox;
	ComponentContainer<BattleEnemy> battleEnemy;
	ComponentContainer<ParticleTimer> particleTimers;
	ComponentContainer<BattleCombo> combos;
	ComponentContainer<BattlePlayer> battlePlayer;
	ComponentContainer<BattleLanes> battleLanes;
	
	// constructor that adds all containers for looping over them
	ECSRegistry()
	{
		// Add all component containers to registry
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&isChasing);
		registry_list.push_back(&isRunning);
		registry_list.push_back(&levels);
		registry_list.push_back(&battleProfiles);
		registry_list.push_back(&pauseEnemyTimers);
		registry_list.push_back(&battleOverPopUpParts);
		registry_list.push_back(&battleReminderPopUpParts);
		registry_list.push_back(&tutorialParts);
		registry_list.push_back(&disabled);
		registry_list.push_back(&foregrounds);
		registry_list.push_back(&backgrounds);
		registry_list.push_back(&motions);
		registry_list.push_back(&notes);
		registry_list.push_back(&noteDurations);
		registry_list.push_back(&players);
		registry_list.push_back(&enemies);
		registry_list.push_back(&conductors);
		registry_list.push_back(&screens);
		registry_list.push_back(&collisions);
		registry_list.push_back(&judgmentLine);
		registry_list.push_back(&collisionTimers);
		registry_list.push_back(&judgmentLineTimers);
		registry_list.push_back(&progressBars);
		registry_list.push_back(&colours);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&texts);
		registry_list.push_back(&staticTexts);
		registry_list.push_back(&dynamicTexts);
		registry_list.push_back(&textTimers);
		registry_list.push_back(&boxButtons);
		registry_list.push_back(&particleEffects);
		registry_list.push_back(&boxAreaBounds);
		registry_list.push_back(&enemyCS);
		registry_list.push_back(&playerCS);
		registry_list.push_back(&CSTexts);
		registry_list.push_back(&CSTextbox);
		registry_list.push_back(&battleEnemy);
		registry_list.push_back(&particleTimers);
		registry_list.push_back(&combos);
		registry_list.push_back(&battlePlayer);
		registry_list.push_back(&battleLanes);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;