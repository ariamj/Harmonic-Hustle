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
	ComponentContainer<Motion> motions;
	ComponentContainer<Note> notes;
	ComponentContainer<Player> players;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Conductor> conductors;
	ComponentContainer<Screen> screens;
	ComponentContainer<Collision> collisions;
	ComponentContainer<JudgementLine> judgmentLine;
	ComponentContainer<CollisionTimer> collisionTimers;
	ComponentContainer<JudgementLineTimer> judgmentLineTimers;
	ComponentContainer<vec3> colours;

	// constructor that adds all containers for looping over them
	ECSRegistry()
	{
		// Add all component containers to registry
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&motions);
		registry_list.push_back(&notes);
		registry_list.push_back(&players);
		registry_list.push_back(&enemies);
		registry_list.push_back(&conductors);
		registry_list.push_back(&screens);
		registry_list.push_back(&collisions);
		registry_list.push_back(&judgmentLine);
		registry_list.push_back(&collisionTimers);
		registry_list.push_back(&judgmentLineTimers);
		registry_list.push_back(&colours);
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