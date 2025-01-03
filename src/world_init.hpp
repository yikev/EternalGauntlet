#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hardcoded to scale the dimensions of the entity texture
const float HERO_WIDTH  = 150.f;
const float HERO_HEIGHT = 180.f;

const float HERO_BASIC_PROJECTILE_WIDTH = 25.f;

const float TACKLE_WIDTH = 255.f;
const float TACKLE_HEIGHT = 180.f;

const float HEAVY_WIDTH = 225.f;
const float HEAVY_HEIGHT = 180.f;

const float BODYSLAM_WIDTH = 255.f;
const float BODYSLAM_HEIGHT = 180.f;

const float LIGHTNING_WIDTH = 80.f;
const float LIGHTNING_HEIGHT = 300.f;

const float ICESHARD_WIDTH = 100.f;
const float ICESHARD_HEIGHT = 25.f;

const float ENEMY_WIDTH   = 200.f;
const float ENEMY_HEIGHT  = 200.f;

const float MELEE_WIDTH = 250.f;
const float MELEE_HEIGHT = 200.f;

const float BOSS_WIDTH = 300.f;
const float BOSS_HEIGHT = 400.f;

const float FIREBALL_WIDTH = 200.f;
const float FIREBALL_HEIGHT = 50.f;

const float UI_ATTACK_BUTTON_WIDTH = 150.f;
const float UI_ATTACK_BUTTON_HEIGHT = 200.f;

const float UI_PLAYER_HEALTH_ICON_WIDTH = 60.f;
const float UI_PLAYER_HEALTH_ICON_HEIGHT = 50.f;

const float UI_PLAYER_MANA_ICON_WIDTH = 20.f;
const float UI_PLAYER_MANA_ICON_HEIGHT = 20.f;

const float UI_LINE_WIDTH = 3000.f;
const float UI_LINE_HEIGHT = 100.f;

const float UI_MAP_BUTTON_WIDTH = 100.f;
const float UI_MAP_BUTTON_HEIGHT = 100.f;

const float UI_TUTORIAL_WIDTH = 600.f;
const float UI_TUTORIAL_HEIGHT = window_height_px;

const vec3 UI_PLAYER_HEALTH_RED = vec3(0.827f, 0.f, 0.f);
const vec3 UI_PLAYER_MANA_BLUE = vec3(0.f, 0.482f, 1.f);

// FPS counter
Entity createUIFPS(RenderSystem* renderer);

// Background
Entity createUIBackground(RenderSystem* renderer, vec2 position);

// Map
Entity createMapWall(RenderSystem* renderer, vec2 position, vec2 scale);

// Menu UI
Entity createUIMapButton(RenderSystem* renderer, vec2 pos);
Entity createUIMenuButton(RenderSystem* renderer, vec2 pos, vec2 scale);
Entity createUILootButton(RenderSystem* renderer, vec2 position);

// Tutorial UI
Entity createUITutorial(RenderSystem* renderer, vec2 pos);
void createTutorialText();
void deleteTutorialText();

// Combat UI
Entity createUIAttackButton(RenderSystem* renderer, vec2 pos, int index, float window_width, float window_height);
Entity createUIPlayerHealth(RenderSystem* renderer);
Entity createUIPlayerHealthIcon(RenderSystem* renderer);
Entity createUIPlayerMana(RenderSystem* renderer);
Entity createUIPlayerManaIcon(RenderSystem* renderer);
Entity createUILine(RenderSystem* renderer, float y);

// Hero (the player)
Entity createHero(RenderSystem* renderer, vec2 pos);

// Enemies
Entity createEnemy(RenderSystem* renderer, vec2 pos, const std::string& enemyType);
Entity createMapEnemy(RenderSystem* renderer, vec2 pos, int combatStage);
Entity createMapBossEnemy(RenderSystem* renderer, vec2 position, int combatStage);
Entity createEnemyWithMesh(RenderSystem* renderer, vec2 position, const std::string& enemyType);
Entity createMeshMapEnemy(RenderSystem* renderer, vec2 position, int combatStage);

// Projectiles
Entity createBasicProjectile(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createTackle(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createHeavy(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createBodyslam(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createFireball(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createLightning(RenderSystem* renderer, Attack attack, Entity target);
Entity createIceShard(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createLifetap(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createEnemyFireball(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createEnemyStar(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);
Entity createEnemyIceSpear(RenderSystem* renderer, vec2 pos, Attack attack, Entity target);

