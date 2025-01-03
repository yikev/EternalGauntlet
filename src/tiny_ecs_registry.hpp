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
	// TODO: A1 add a LightUp component
	ComponentContainer<Hit> hits;
	ComponentContainer<Player> players;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Health> healths;
	ComponentContainer<Mana> manas;
	ComponentContainer<FPS> fpss;
	ComponentContainer<AttackComponent> attacks;
	ComponentContainer<Level> levels;
	ComponentContainer<Damage> damages;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<RenderRequest> renderRequestsMap;
	ComponentContainer<RenderRequest> renderRequestsMenu;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<vec3> colors;
	ComponentContainer<Modifier> modifiers;
	ComponentContainer<Image> images;
	ComponentContainer<Motion> motions;
	ComponentContainer<Motion> mapMotions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Eatable> eatables;
	ComponentContainer<Deadly> deadlys;
	ComponentContainer<Attacker> attackers;
	ComponentContainer<Parry> parrys;
	ComponentContainer<ParryCooldown> parryCooldowns;
	ComponentContainer<UI> uis;
	ComponentContainer<EnemyType> enemyTypes;
	ComponentContainer<Target> targets;
	ComponentContainer<UIText> texts;
	ComponentContainer<UIText> mapTutorialTexts;
	ComponentContainer<UIText> combatTutorialTexts;
	ComponentContainer<Map> maps;
	ComponentContainer<Buttons> buttons;
	ComponentContainer<MapEnemy> mapEnemies;
	ComponentContainer<MenuButton> menuButtons;
	ComponentContainer<Menu> menus;
	ComponentContainer<Wall> walls;
	ComponentContainer<Projectile> projectiles;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&hits);
		registry_list.push_back(&players);
		registry_list.push_back(&enemies);
		registry_list.push_back(&healths);
		registry_list.push_back(&manas);
		registry_list.push_back(&fpss);
		registry_list.push_back(&attacks);
		registry_list.push_back(&levels);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&renderRequestsMap);
		registry_list.push_back(&renderRequestsMenu);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&colors);
		registry_list.push_back(&modifiers);
		registry_list.push_back(&images);
		registry_list.push_back(&damages);
		registry_list.push_back(&motions);
		registry_list.push_back(&mapMotions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&eatables);
		registry_list.push_back(&deadlys);
		registry_list.push_back(&attackers);
		registry_list.push_back(&parrys);
		registry_list.push_back(&uis);
		registry_list.push_back(&parryCooldowns);
		registry_list.push_back(&enemyTypes);
		registry_list.push_back(&targets);
		registry_list.push_back(&texts);
		registry_list.push_back(&mapTutorialTexts);
		registry_list.push_back(&combatTutorialTexts);
    	registry_list.push_back(&maps);
		registry_list.push_back(&buttons);
		registry_list.push_back(&mapEnemies);
		registry_list.push_back(&menuButtons);
		registry_list.push_back(&menus);
		registry_list.push_back(&walls);
		registry_list.push_back(&projectiles);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all components:\n");
		for (ContainerInterface* reg : registry_list) {
			size_t count = reg->size();
			std::string typeName = typeid(*reg).name();
			
			// Match the typeid name to a human-readable string
			if (typeName.find("ComponentContainer<Hit>") != std::string::npos) {
				printf(" - %zu components of type Hit\n", count);
			} else if (typeName.find("ComponentContainer<Player>") != std::string::npos) {
				printf(" - %zu components of type Player\n", count);
			} else if (typeName.find("ComponentContainer<Health>") != std::string::npos) {
				printf(" - %zu components of type Health\n", count);
			} else if (typeName.find("ComponentContainer<Mana>") != std::string::npos) {
				printf(" - %zu components of type Mana\n", count);
			} else if (typeName.find("ComponentContainer<FPS>") != std::string::npos) {
				printf(" - %zu components of type FPS\n", count);
			} else if (typeName.find("ComponentContainer<AttackComponent>") != std::string::npos) {
				printf(" - %zu components of type AttackComponent\n", count);
			} else if (typeName.find("ComponentContainer<RenderRequest>") != std::string::npos) {
				printf(" - %zu components of type RenderRequest\n", count);
			} else if (typeName.find("ComponentContainer<ScreenState>") != std::string::npos) {
				printf(" - %zu components of type ScreenState\n", count);
			} else if (typeName.find("ComponentContainer<vec3>") != std::string::npos) {
				printf(" - %zu components of type vec3 (glm::vec3)\n", count);
			} else if (typeName.find("ComponentContainer<Motion>") != std::string::npos) {
				printf(" - %zu components of type Motion\n", count);
			} else if (typeName.find("ComponentContainer<UI>") != std::string::npos) {
				printf(" - %zu components of type UI\n", count);
			} else if (typeName.find("ComponentContainer<UIText>") != std::string::npos) {
				printf(" - %zu components of type UIText\n", count);
			} else {
				printf(" - %zu components of unknown type (%s)\n", count, typeName.c_str());
			}
		}
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;
