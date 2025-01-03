#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "enemy_attacks.hpp"

Entity createUIAttackButton(RenderSystem* renderer, vec2 position, int index, float window_width, float window_height)
{
	// Reserve an entity
	auto entity = Entity();
	Entity player = registry.players.entities[0];

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	auto& button = registry.buttons.emplace(entity);
	ui.position = position;
	ui.scale = vec2({ UI_ATTACK_BUTTON_WIDTH, UI_ATTACK_BUTTON_HEIGHT });
	ui.type = UI_TYPE::UI_ATTACK_BUTTON;

	float font_pos_x = -70.f;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
			entity,
			{
				TEXTURE_ASSET_ID::UI_ATTACK_BUTTON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE
			});
	registry.texts.insert(entity,
			{
				UI_TYPE::UI_BASIC,
				entity,
				registry.attacks.get(player).availableAttacks[index].name,
				position + vec2(font_pos_x, -510.f), // need to adjust for different projection matrix
				0.4f,
				vec3(0.f, 0.f, 0.f)
			});
	entity = Entity();
	registry.texts.insert(entity,
			{
				UI_TYPE::UI_BASIC,
				entity,
				std::to_string(registry.attacks.get(player).availableAttacks[index].damage) + " dmg",
				position + vec2(font_pos_x, -560.f), // need to adjust for different projection matrix
				0.4f,
				UI_PLAYER_HEALTH_RED
			});
	entity = Entity();
	registry.texts.insert(entity,
			{
				UI_TYPE::UI_BASIC,
				entity,
				std::to_string(registry.attacks.get(player).availableAttacks[index].manaCost) + " mana",
				position + vec2(font_pos_x, -610.f), // need to adjust for different projection matrix
				0.4f,
				UI_PLAYER_MANA_BLUE
			});
	entity = Entity();
	registry.texts.insert(entity,
			{
				UI_TYPE::UI_BASIC,
				entity,
				std::to_string(registry.attacks.get(player).availableAttacks[index].heal) + " heal",
				position + vec2(font_pos_x, -660.f), // need to adjust for different projection matrix
				0.4f,
				UI_PLAYER_HEALTH_RED
			});

	return entity;
}

Entity createUIPlayerHealth(RenderSystem* renderer)
{
	// Reserve an entity
	auto entity = Entity();
	Entity player = registry.players.entities[0]; // get link to player entity

	registry.texts.insert(entity,
		{
			UI_TYPE::UI_HEALTH,
			player,
			"",
			vec2(110.f, window_height_px - 50.f),
			0.8f,
			UI_PLAYER_HEALTH_RED
		});

	return entity;
}

Entity createUIPlayerMana(RenderSystem* renderer)
{
	// Reserve an entity
	auto entity = Entity();
	Entity player = registry.players.entities[0]; // get link to player entity

	registry.texts.insert(entity,
		{
			UI_TYPE::UI_MANA,
			player,
			"",
			vec2(window_width_px/2 - 5 - 75 - 150 - 10 - 35, window_height_px - 110.f - 470.f),
			0.4f,
			UI_PLAYER_MANA_BLUE
		});

	return entity;
}

Entity createUIFPS(RenderSystem* renderer) {
	// Reserve an entity
	auto entity = Entity();
	Entity player = registry.players.entities[0]; // get link to player entity

	registry.texts.insert(entity,
		{
			UI_TYPE::UI_FPS,
			player,
			"",
			vec2(window_width_px - 50 , window_height_px - 50),
			0.4f,
			UI_PLAYER_MANA_BLUE
		});

	return entity;
}

Entity createUIPlayerHealthIcon(RenderSystem* renderer)
{
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = vec2({ 60.f, 35.f });
	ui.scale = vec2({ UI_PLAYER_HEALTH_ICON_WIDTH, UI_PLAYER_HEALTH_ICON_HEIGHT });
	ui.type = UI_TYPE::UI_BASIC;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
			entity,
			{
				TEXTURE_ASSET_ID::UI_PLAYER_HEALTH_ICON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE
			});

	return entity;
}


Entity createUIPlayerManaIcon(RenderSystem* renderer)
{
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = vec2(window_width_px/2 - 5 - 75 - 150 - 10 - 55, window_height_px - 110.f - 118.f);
	ui.scale = vec2({ UI_PLAYER_MANA_ICON_WIDTH, UI_PLAYER_MANA_ICON_HEIGHT });
	ui.type = UI_TYPE::UI_BASIC;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
			entity,
			{
				TEXTURE_ASSET_ID::UI_PLAYER_MANA_ICON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE
			});

	return entity;
}

Entity createUILine(RenderSystem* renderer, float y)
{
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = vec2(window_width_px/2, y);
	ui.scale = vec2({ UI_LINE_WIDTH, UI_LINE_HEIGHT });
	ui.type = UI_TYPE::UI_BASIC;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
			entity,
			{
				TEXTURE_ASSET_ID::UI_ATTACK_BUTTON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE
			});

	return entity;
}

Entity createUIMapButton(RenderSystem* renderer, vec2 position)
{
	// Reserve an entity
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = position;
	ui.scale = vec2({ UI_MAP_BUTTON_WIDTH, UI_MAP_BUTTON_HEIGHT });
	ui.type = UI_TYPE::UI_MAP_BUTTON;
	ui.enemy_hp = false;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::UI_MAP_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createUILootButton(RenderSystem* renderer, vec2 position)
{
	// Reserve an entity
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = position;
	ui.scale = vec2({ UI_MAP_BUTTON_WIDTH, UI_MAP_BUTTON_HEIGHT });
	ui.type = UI_TYPE::UI_LOOT;
	ui.enemy_hp = false;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::UI_LOOT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createUIMenuButton(RenderSystem* renderer, vec2 position, vec2 scale)
{
	// Reserve an entity
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = position;
	ui.scale = vec2(scale);
	ui.type = UI_TYPE::UI_MENU;
	ui.enemy_hp = false;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
			entity,
			{
				TEXTURE_ASSET_ID::UI_MENU,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE
			});

	return entity;
}

Entity createUITutorial(RenderSystem* renderer, vec2 position)
{
	// Reserve an entity
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = position;
	ui.scale = vec2({ UI_TUTORIAL_WIDTH+100, UI_TUTORIAL_HEIGHT });
	ui.type = UI_TYPE::UI_TUTORIAL;
	ui.enemy_hp = false;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::UI_TUTORIAL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createEnemyWithMesh(RenderSystem* renderer, vec2 position, const std::string& enemyType) {
  auto entity = Entity();

  // Initialize motion, mesh, etc. similar to createSalmon
  Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0 };
	motion.position = position;
	motion.originalPosition = position;

	motion.scale = vec2({ ENEMY_WIDTH, ENEMY_HEIGHT });

	registry.enemyTypes.emplace(entity, EnemyType(enemyType));
	registry.enemies.emplace(entity);
	registry.healths.emplace(entity, Health{100, 100}); // Enemy starts with 100 HP
	registry.hits.emplace(entity, Hit{ false });

	std::vector<Attack> attacks = getAvailableAttacksForEnemyType(enemyType);
	registry.attacks.emplace(entity, AttackComponent{attacks});

    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT,  // Use enemy-specific texture
          EFFECT_ASSET_ID::SALMON,
          GEOMETRY_BUFFER_ID::SALMON });
    return entity;
}

Entity createEnemy(RenderSystem* renderer, vec2 position, const std::string& enemyType) {
	auto entity = Entity();

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0 };
	motion.position = position;
	motion.originalPosition = position;

	motion.scale = vec2({ ENEMY_WIDTH, ENEMY_HEIGHT });
	motion.hitBoxScale = vec2({ ENEMY_WIDTH, ENEMY_HEIGHT });

	if (enemyType == "Boss") {
		motion.scale = vec2({ BOSS_WIDTH, BOSS_HEIGHT });
		motion.hitBoxScale = vec2({ BOSS_WIDTH, BOSS_HEIGHT });
	} else if (enemyType == "Melee") {
		motion.scale = vec2({ MELEE_WIDTH, MELEE_HEIGHT });
		motion.hitBoxScale = vec2({ MELEE_WIDTH, MELEE_HEIGHT });
	}

	registry.enemyTypes.emplace(entity, EnemyType(enemyType));
	Enemy& enemy = registry.enemies.emplace(entity);
	if (enemyType == "Boss") {
		registry.healths.emplace(entity, Health(500, 500));
	}
	else if (enemyType == "Tank") {
		registry.healths.emplace(entity, Health(200, 200));
	}
	else {
		registry.healths.emplace(entity, Health(100, 100));
	}

	registry.hits.emplace(entity, Hit{ false });

	std::vector<Attack> attacks = getAvailableAttacksForEnemyType(enemyType);
	registry.attacks.emplace(entity, AttackComponent{ attacks });
	
	if (enemyType == "Tank") {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TANK,  // Use enemy-specific texture
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (enemyType == "Healer") {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::HEALER,  // Use enemy-specific texture
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	} 
	else if (enemyType == "Ranged") {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::RANGED,  // Use enemy-specific texture
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (enemyType == "Melee") {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::MELEE,  // Use enemy-specific texture
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS,  // Use enemy-specific texture
			  EFFECT_ASSET_ID::TEXTURED,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}

	// Reserve an entity
	auto text = Entity();

	vec2 healthPosition = { position.x - motion.scale.x / 2, window_height_px - position.y + motion.scale.y / 2 };

	if (enemyType == "Boss") {
		healthPosition = { position.x - motion.scale.x / 4, window_height_px - position.y + motion.scale.y / 2 };
	}

	registry.texts.insert(text,
		{
			UI_TYPE::UI_HEALTH,
			entity,
			"",
			healthPosition,
			0.5f,
			UI_PLAYER_HEALTH_RED
		});

	return entity;
}

Entity createMapEnemy(RenderSystem* renderer, vec2 position, int combatStage) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.originalPosition = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ ENEMY_WIDTH, ENEMY_HEIGHT });
	motion.hitBoxScale = vec2({ ENEMY_WIDTH, ENEMY_HEIGHT });

	if (combatStage == 3) {
		motion.scale = vec2({ BOSS_WIDTH, BOSS_HEIGHT });
		motion.hitBoxScale = vec2({ BOSS_WIDTH, BOSS_HEIGHT });
	}

	registry.hits.emplace(entity, Hit{ false });
	MapEnemy& mapEnemy = registry.mapEnemies.emplace(entity);
	mapEnemy.combatStage = combatStage;

	if (combatStage == 4) {
		registry.renderRequestsMap.insert(
			entity,
			{ TEXTURE_ASSET_ID::TANK, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (combatStage == 1){
		registry.renderRequestsMap.insert(
			entity,
			{ TEXTURE_ASSET_ID::RANGED, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (combatStage == 3){
		registry.renderRequestsMap.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequestsMap.insert(
			entity,
			{ TEXTURE_ASSET_ID::MELEE, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	}

	return entity;
}

Entity createHero(RenderSystem* renderer, vec2 position) {
    auto entity = Entity();

		/* TODO: Use mesh model for hero
    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
		 */

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
    motion.position = position;
	motion.originalPosition = position;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ HERO_WIDTH, HERO_HEIGHT });
	motion.hitBoxScale = vec2({ HERO_WIDTH, HERO_HEIGHT });

		/* TODO: Use mesh model for hero
    motion.scale = mesh.original_size * 300.f;
    motion.scale.y *= -1; // point front to the right
		motion.scale.x *= -1;
		 */

    registry.healths.emplace(entity, Health{150,150});
	registry.manas.emplace(entity, Mana{100, 100});
	registry.fpss.emplace(entity, FPS{ 0 });

    // Attach specific hero properties, like mana or abilities
    registry.players.emplace(entity);

    // Add multiple attacks for the hero
	// name(n), damage(dmg), manaCost(mana), affectedEnemy(affectedEnemy), hits(hits), heal(heal)
    registry.attacks.emplace(entity, AttackComponent{
        {Attack{"Basic", 15, 0, 1, 1,15, false}, Attack{"Tackle", 25, 20, 1, 1,0, false}, Attack{"Heavy", 30, 35, 1, 1,0, false}, Attack{"BodySlam", 50, 100, 1, 1,0, false}}
    });

	registry.hits.emplace(entity, Hit{ false });
	AttackComponent& AttackComponent = registry.attacks.get(entity);
	AttackComponent.selectedAttackIndex = 0;
	Attack& attack = AttackComponent.getCurrentAttack();
	// attack.modifiers.push_back(Modifier(ModifierType::ADDDAMAGE, std::string("Increase damage by 5"), 5));

	AttackComponent.selectedAttackIndex = 1;
	Attack& attackTwo = AttackComponent.getCurrentAttack();
	// attackTwo.modifiers.push_back(Modifier(ModifierType::MULTIPLYDAMAGE, std::string("Multiply damage by 2"), 2));

    // Add hero-specific render request
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::HERO_IDLE_0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });

	registry.renderRequestsMap.insert(
		entity,
		{ TEXTURE_ASSET_ID::HERO_IDLE_0, EFFECT_ASSET_ID::TEXTURED, GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createUIBackground(RenderSystem* renderer, vec2 position)
{
	// Reserve an entity
	auto entity = Entity();

	// Initialize ui component (for rendering)
	auto& ui = registry.uis.emplace(entity);
	ui.position = position;
	ui.scale = vec2({ window_width_px, window_height_px });
	ui.type = UI_TYPE::UI_BACKGROUND;
	ui.enemy_hp = false;

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::UI_BACKGROUND,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createMapWall(RenderSystem* renderer, vec2 position, vec2 scale)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = { position.x * window_width_px, position.y * window_height_px };
	motion.originalPosition = { position.x * window_width_px, position.y * window_height_px };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ scale.x * window_width_px, scale.y * window_height_px });
	motion.hitBoxScale = vec2({ scale.x * window_width_px, scale.y * window_height_px });

	registry.hits.emplace(entity, Hit{ false });
	registry.walls.emplace(entity);

	registry.renderRequestsMap.insert(
		entity,
		{
			TEXTURE_ASSET_ID::UI_WALL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	return entity;
}

Entity createBasicProjectile(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.originalPosition = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.angle = 0.f;
	motion.velocity = { 400.f, 0.f};
	motion.scale = vec2({HERO_BASIC_PROJECTILE_WIDTH, HERO_BASIC_PROJECTILE_WIDTH});


	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_BASIC_PROJECTILE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

Entity createTackle(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.originalPosition = pos;
	motion.angle = 0.f;
	motion.velocity = { 400.f, 0.f};
	motion.scale = vec2({TACKLE_WIDTH, TACKLE_HEIGHT});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_TACKLE_0,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = true;
	return entity;
}

Entity createHeavy(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.originalPosition = pos;
	motion.angle = 0.f;
	motion.velocity = { 400.f, 0.f};
	motion.scale = vec2({HEAVY_WIDTH, HEAVY_HEIGHT});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_HEAVY_0,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = true;
	return entity;
}

Entity createBodyslam(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.originalPosition = pos;
	motion.angle = 0.f;
	motion.velocity = { 400.f, 0.f};
	motion.scale = vec2({BODYSLAM_WIDTH, BODYSLAM_HEIGHT});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_BODYSLAM_0,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = true;
	return entity;
}

Entity createFireball(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.originalPosition = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.angle = 0.f;
	motion.velocity = { 400.f, 0.f};
	motion.scale = vec2({FIREBALL_WIDTH, FIREBALL_HEIGHT});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_FIREBALL_PROJECTILE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

Entity createLightning(RenderSystem* renderer, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2({registry.motions.get(target).position.x, 0});
	motion.originalPosition = vec2({registry.motions.get(target).position.x, 0});
	motion.angle = 0.f;
	motion.velocity = { 0.f, 400.f};
	motion.scale = vec2({LIGHTNING_WIDTH, LIGHTNING_HEIGHT});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_LIGHTNING_PROJECTILE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

Entity createIceShard(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.originalPosition = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.angle = 0.f;
	motion.velocity = { 400.f, 0.f};
	motion.scale = vec2({ICESHARD_WIDTH, ICESHARD_HEIGHT});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_ICESHARD_PROJECTILE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

Entity createLifetap(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.originalPosition = pos + vec2({HERO_WIDTH/2, -20.f});
	motion.angle = 0.f;
	motion.velocity = { 400.f, 0.f};
	motion.scale = vec2({HERO_BASIC_PROJECTILE_WIDTH * 3, HERO_BASIC_PROJECTILE_WIDTH * 3});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::HERO_LIFETAP_PROJECTILE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target;
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

Entity createEnemyFireball(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.originalPosition = pos;
	motion.angle = 0.f;
	motion.velocity = { -400.f, 0.f};
	motion.scale = vec2({FIREBALL_WIDTH, FIREBALL_HEIGHT});

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{
			TEXTURE_ASSET_ID::FIREBALL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE
		});

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target; // target player
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

Entity createEnemyStar(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.originalPosition = pos;
	motion.angle = 0.f;
	motion.velocity = { -400.f, 0.f };
	motion.scale = vec2({ FIREBALL_WIDTH, FIREBALL_HEIGHT });

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,  // Use enemy-specific texture
		  EFFECT_ASSET_ID::SALMON,
		  GEOMETRY_BUFFER_ID::SALMON });

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target; // target player
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

Entity createEnemyIceSpear(RenderSystem* renderer, vec2 pos, Attack attack, Entity target)
{
	// Reserve an entity
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.originalPosition = pos;
	motion.angle = 0.f;
	motion.velocity = { -400.f, 0.f };
	motion.scale = vec2({ FIREBALL_WIDTH, FIREBALL_HEIGHT * 3 });

	registry.hits.emplace(entity, Hit{ false });
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ICE_SPEAR,  // Use enemy-specific texture
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	Projectile& projectile = registry.projectiles.emplace(entity);
	projectile.target = target; // target player
	projectile.attack = attack;
	projectile.isMelee = false;
	return entity;
}

void createTutorialText() {

	Entity entity = Entity();
	registry.mapTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"PRESS ARROW KEYS TO MOVE!",
			{ window_width_px * 0.1, window_height_px * 0.2 },
			0.4f,
			{0,0,0}
		});

	entity = Entity();
	registry.mapTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"COLLIDING WITH AN ENEMY STARTS COMBAT!",
			{ window_width_px * 0.65, window_height_px * 0.2 },
			0.4f,
			{0,0,0}
		});

	entity = Entity();
	registry.combatTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"YOUR ATTACKS",
			{ window_width_px * 0.45, window_height_px * .275 },
			0.4f,
			{0,0,0}
		});

	entity = Entity();
	registry.combatTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"CLICK AN ATTACK AND THEN CLICK AN ENEMY TO ATTACK!",
			{ window_width_px * 0.285, window_height_px * .375 },
			0.4f,
			{0,0,0}
		});

	entity = Entity();
	registry.combatTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"YOU",
			{ window_width_px * 0.185, window_height_px * .65 },
			0.4f,
			{0,0,0}
		});

	entity = Entity();
	registry.combatTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"ENEMIES",
			{ window_width_px * 0.75, window_height_px * .7 },
			0.4f,
			{0,0,0}
		});

	entity = Entity();
	registry.combatTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"YOUR MANA, IF YOU CAN'T USE AN ATTACK BECAUSE OF MANA ISSUES YOU LOSE!",
			{ window_width_px * 0.2, window_height_px * .3 },
			0.4f,
			{0,0,0}
		});


	entity = Entity();
	registry.combatTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"YOUR HEALTH",
			{ window_width_px * 0.085, window_height_px * .91 },
			0.4f,
			{0,0,0}
		});

	entity = Entity();
	registry.mapTutorialTexts.insert(entity,
		{
			UI_TYPE::UI_BASIC,
			entity,
			"CLICKING M BRINGS YOU BACK TO THE MENU",
			{ window_width_px * 0.05, window_height_px * .9 },
			0.4f,
			{0,0,0}
		});
}

void deleteTutorialText() {
	for (Entity mapEntity : registry.mapTutorialTexts.entities) {
		registry.mapTutorialTexts.remove(mapEntity);
	}

	for (Entity mapEntity : registry.combatTutorialTexts.entities) {
		registry.combatTutorialTexts.remove(mapEntity);
	}
}