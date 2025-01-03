#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

enum class ModifierType {
	NONE,
	ADDDAMAGE,
	DECREASEDAMAGE,
	DIVIDEDAMAGE,
	MULTIPLYDAMAGE,
	DECREASEMANACOST,
	INCREASEMANACOST,
	SLOWENEMY,
	SPEEEDENEMY,
	DISARMENEMY
};

enum class AnimationType {
	IDLE,
	BASIC,
	HERO_MELEE,
	FIREBALL,
	LIGHTNING,
	ICESHARD,
	ZAP,
	QUAKE,
	LIFETAP,
	PUNCH,
	HEAVYPUNCH,
	HEAL,
	SPELL,
	RANGED,
	MELEE_PUNCH,
	SLASH,
	HEAVY_HIT
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	vec2 hitBoxScale;
	vec2 originalPosition = { 0, 0 };
	vec2 mapPosition = { 0, 0 };
	Entity mapEnemy;
};

struct Room {
	int roomEnemies;
	Room* leftroom;
	Room* uproom;
	Room* downroom;
	Room* rightroom;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// anything that is deadly to the player
struct Deadly
{

};

struct Buttons
{

};

struct Target
{

};

struct Parry
{

};

struct ParryCooldown
{

};

// anything that is deadly to the player
struct Attacker
{

};

// anything the player can eat
struct Eatable
{

};

struct MapEnemy 
{
	int combatStage;
};

struct Wall {

};

// Player component
struct Player
{
	int mana = 100;
	AnimationType anim_type = AnimationType::IDLE;
};

// Enemy component
struct Enemy
{
	int mana = 100;
	float timer_since_last_change = 0.f;
	AnimationType anim_type = AnimationType::IDLE;
};

struct Level {
	int number;
	int height;
	int enemies;
	std::string type;
    std::vector<int> connections;
};

struct Map {
    std::unordered_map<int, Level> levels;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct Health {
	int hp;
	int max_hp;
	bool took_damage = false;

	Health(int hp, int max_hp)
		: hp(hp), max_hp(max_hp), took_damage(false) {}
};

struct Mana {
    int currentMana;
    int maxMana;
};

struct FPS {
	int fps = 0;
};

struct Damage {
    int amount = 0;
};

struct Hit {
	bool hit = false;
};

struct Modifier {
	ModifierType modifierType;
	// Probably have an enum in the future to determine the "type" of the effect which does different things
	std::string effect = "";
	// The value that we use to calculate the change it makes on an attack ie if the effect is times X damage we use this value for calculations
	float effectModifier = 0.f;

	Modifier(const ModifierType& modifiertype, std::string effect, float effectModifier)
		: modifierType(modifiertype), effect(effect), effectModifier(effectModifier) {}
};

struct Attack {
	std::vector<Modifier> modifiers;
    std::string name;
    int damage;
    int manaCost;
	int affectedEnemy;
	int hits;
	int heal;
	int maxModifiers = 1;
	int usedHits = 1;
	bool isRanged = false;

    Attack()
        : name("Default"), damage(0), manaCost(0), affectedEnemy(0), hits(0), heal(0), isRanged(false) {}
    
    Attack(const std::string& n, int dmg, int mana, int affectedEnemy, int hits, int heal, bool isRanged)
        : name(n), damage(dmg), manaCost(mana), affectedEnemy(affectedEnemy), hits(hits), heal(heal), isRanged(isRanged) {}
};

struct Projectile {
	Entity target;
	Attack attack;
	bool isMelee = false;
};

struct PlayerSaveData {
    int currentHp;
    int maxHp;
    int currentMana;
    int maxMana;
    std::vector<Attack> attacks;
};

struct EnemyType {
    std::string type;

    // Constructor to initialize the type
    EnemyType(const std::string& type) : type(type) {}
};

struct AttackComponent {
    std::vector<Attack> availableAttacks;
    int selectedAttackIndex = 0;  // Index of the currently selected attack
	Attack dummyAttack = {"None", 0, 0, 0, 0, 0, false};

    // Method to get the current selected attack
    Attack& getCurrentAttack() {
		if (selectedAttackIndex == -1) {
			return dummyAttack;
		}

        return availableAttacks[selectedAttackIndex];
    }

    // Method to swap attacks
    void selectAttack(int index) {
        selectedAttackIndex = index;
    }
};

struct LootOption {
    std::string name;
    std::string description;
    std::function<void()> voidEffect;  // For effects that do not return values
    std::function<Attack()> attackEffect;  // For effects that return an Attack
};

struct MenuButton {
	std::string name;
};

struct Menu {
	std::string title;
	std::vector<MenuButton> buttons;
};

struct Image {
	std::string img = "";
};

enum class UI_TYPE {
	UI_BASIC = 0,
	UI_HP_BAR = UI_BASIC + 1,
	UI_HEALTH = UI_HP_BAR + 1,
	UI_MANA = UI_HEALTH + 1,
	UI_MAP_BUTTON = UI_MANA + 1,
	UI_ATTACK_BUTTON = UI_MAP_BUTTON + 1,
	UI_FPS = UI_MAP_BUTTON + 1,
	UI_TUTORIAL = UI_FPS + 1,
	UI_FIRE = UI_TUTORIAL + 1,
	UI_BACKGROUND = UI_FIRE + 1,
	UI_LOOT = UI_BACKGROUND + 1,
	UI_WALL = UI_LOOT + 1,
	UI_MENU = UI_WALL + 1,

};
typedef enum UI_TYPE UI_TYPE;

struct UI {
	vec2 position = { 0, 0 };
	vec2 scale = { 10, 10 };
	UI_TYPE type = UI_TYPE::UI_BASIC;
	bool enemy_hp = false;
};

struct UIText {
	UI_TYPE type;
	Entity link;
	std::string text;
	vec2 position;
	float scale;
	vec3 color;
};

/*
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	HERO_IDLE_0 = 0,
	HERO_IDLE_1 = HERO_IDLE_0 + 1,
	HERO_IDLE_2 = HERO_IDLE_1 + 1,
	HERO_IDLE_3 = HERO_IDLE_2 + 1,
	EMPTY = HERO_IDLE_3 + 1,
	HERO_BASIC_0 = EMPTY + 1,
	HERO_BASIC_1 = HERO_BASIC_0 + 1,
	HERO_BASIC_2 = HERO_BASIC_1 + 1,
	HERO_BASIC_3 = HERO_BASIC_2 + 1,
	HERO_BASIC_4 = HERO_BASIC_3 + 1,
	HERO_BASIC_5 = HERO_BASIC_4 + 1,
	HERO_BASIC_6 = HERO_BASIC_5 + 1,
	HERO_BASIC_7 = HERO_BASIC_6 + 1,
	HERO_BASIC_PROJECTILE = HERO_BASIC_7 + 1,
	HERO_TACKLE_0 = HERO_BASIC_PROJECTILE + 1,
	HERO_HEAVY_0 = HERO_TACKLE_0 + 1,
	HERO_BODYSLAM_0 = HERO_HEAVY_0 + 1,
	HERO_FIREBALL_0 = HERO_BODYSLAM_0 + 1,
	HERO_FIREBALL_1 = HERO_FIREBALL_0 + 1,
	HERO_FIREBALL_2 = HERO_FIREBALL_1 + 1,
	HERO_FIREBALL_3 = HERO_FIREBALL_2 + 1,
	HERO_FIREBALL_4 = HERO_FIREBALL_3 + 1,
	HERO_FIREBALL_5 = HERO_FIREBALL_4 + 1,
	HERO_FIREBALL_6 = HERO_FIREBALL_5 + 1,
	HERO_FIREBALL_7 = HERO_FIREBALL_6 + 1,
	HERO_FIREBALL_8 = HERO_FIREBALL_7 + 1,
	HERO_FIREBALL_9 = HERO_FIREBALL_8 + 1,
	HERO_FIREBALL_10 = HERO_FIREBALL_9 + 1,
	HERO_FIREBALL_11 = HERO_FIREBALL_10 + 1,
	HERO_FIREBALL_12 = HERO_FIREBALL_11 + 1,
	HERO_FIREBALL_13 = HERO_FIREBALL_12 + 1,
	HERO_FIREBALL_14 = HERO_FIREBALL_13 + 1,
	HERO_FIREBALL_PROJECTILE = HERO_FIREBALL_14 + 1,
	HERO_LIGHTNING_0 = HERO_FIREBALL_PROJECTILE + 1,
	HERO_LIGHTNING_1 = HERO_LIGHTNING_0 + 1,
	HERO_LIGHTNING_2 = HERO_LIGHTNING_1 + 1,
	HERO_LIGHTNING_3 = HERO_LIGHTNING_2 + 1,
	HERO_LIGHTNING_4 = HERO_LIGHTNING_3 + 1,
	HERO_LIGHTNING_5 = HERO_LIGHTNING_4 + 1,
	HERO_LIGHTNING_6 = HERO_LIGHTNING_5 + 1,
	HERO_LIGHTNING_7 = HERO_LIGHTNING_6 + 1,
	HERO_LIGHTNING_8 = HERO_LIGHTNING_7 + 1,
	HERO_LIGHTNING_9 = HERO_LIGHTNING_8 + 1,
	HERO_LIGHTNING_10 = HERO_LIGHTNING_9 + 1,
	HERO_LIGHTNING_PROJECTILE = HERO_LIGHTNING_10 + 1,
	HERO_ICESHARD_0 = HERO_LIGHTNING_PROJECTILE + 1,
	HERO_ICESHARD_1 = HERO_ICESHARD_0 + 1,
	HERO_ICESHARD_2 = HERO_ICESHARD_1 + 1,
	HERO_ICESHARD_3 = HERO_ICESHARD_2 + 1,
	HERO_ICESHARD_4 = HERO_ICESHARD_3 + 1,
	HERO_ICESHARD_5 = HERO_ICESHARD_4 + 1,
	HERO_ICESHARD_6 = HERO_ICESHARD_5 + 1,
	HERO_ICESHARD_7 = HERO_ICESHARD_6 + 1,
	HERO_ICESHARD_8 = HERO_ICESHARD_7 + 1,
	HERO_ICESHARD_PROJECTILE = HERO_ICESHARD_8 + 1,
	HERO_ZAP_0 = HERO_ICESHARD_PROJECTILE + 1,
	HERO_ZAP_1 = HERO_ZAP_0 + 1,
	HERO_ZAP_2 = HERO_ZAP_1 + 1,
	HERO_ZAP_3 = HERO_ZAP_2 + 1,
	HERO_ZAP_4 = HERO_ZAP_3 + 1,
	HERO_ZAP_5 = HERO_ZAP_4 + 1,
	HERO_ZAP_6 = HERO_ZAP_5 + 1,
	HERO_QUAKE_0 = HERO_ZAP_6 + 1,
	HERO_QUAKE_1 = HERO_QUAKE_0 + 1,
	HERO_QUAKE_2 = HERO_QUAKE_1 + 1,
	HERO_QUAKE_3 = HERO_QUAKE_2 + 1,
	HERO_QUAKE_4 = HERO_QUAKE_3 + 1,
	HERO_QUAKE_5 = HERO_QUAKE_4 + 1,
	HERO_LIFETAP_0 = HERO_QUAKE_5 + 1,
	HERO_LIFETAP_1 = HERO_LIFETAP_0 + 1,
	HERO_LIFETAP_2 = HERO_LIFETAP_1 + 1,
	HERO_LIFETAP_3 = HERO_LIFETAP_2 + 1,
	HERO_LIFETAP_4 = HERO_LIFETAP_3 + 1,
	HERO_LIFETAP_5 = HERO_LIFETAP_4 + 1,
	HERO_LIFETAP_6 = HERO_LIFETAP_5 + 1,
	HERO_LIFETAP_7 = HERO_LIFETAP_6 + 1,
	HERO_LIFETAP_PROJECTILE = HERO_LIFETAP_7 + 1,
	BOSS = HERO_LIFETAP_PROJECTILE + 1,
	BOSS_FIREBALL_0 = BOSS + 1,
	BOSS_FIREBALL_0_1 = BOSS_FIREBALL_0 + 1,
	BOSS_FIREBALL_0_2 = BOSS_FIREBALL_0_1 + 1,
	BOSS_FIREBALL_1 = BOSS_FIREBALL_0_2 + 1,
	BOSS_FIREBALL_1_1 = BOSS_FIREBALL_1 + 1,
	BOSS_FIREBALL_1_2 = BOSS_FIREBALL_1_1 + 1,
	BOSS_FIREBALL_1_3 = BOSS_FIREBALL_1_2 + 1,
	BOSS_FIREBALL_2 = BOSS_FIREBALL_1_3 + 1,
	BOSS_FIREBALL_2_1 = BOSS_FIREBALL_2 + 1,
	BOSS_FIREBALL_2_2 = BOSS_FIREBALL_2_1 + 1,
	BOSS_FIREBALL_2_3 = BOSS_FIREBALL_2_2 + 1,
	BOSS_FIREBALL_2_4 = BOSS_FIREBALL_2_3 + 1,
	BOSS_FIREBALL_2_5 = BOSS_FIREBALL_2_4 + 1,
	BOSS_FIREBALL_2_6 = BOSS_FIREBALL_2_5 + 1,
	BOSS_FIREBALL_2_7 = BOSS_FIREBALL_2_6 + 1,
	BOSS_FIREBALL_3 = BOSS_FIREBALL_2_7 + 1,
	BOSS_FIREBALL_4 = BOSS_FIREBALL_3 + 1,
	FIREBALL = BOSS_FIREBALL_4 + 1,
	DEAD = FIREBALL + 1,
	UI_ATTACK_BUTTON = DEAD + 1,
	UI_HP_BAR_HERO = UI_ATTACK_BUTTON + 1,
	UI_HP_BAR_ENEMY = UI_HP_BAR_HERO + 1,
	UI_PLAYER_HEALTH_ICON = UI_HP_BAR_ENEMY + 1,
	UI_PLAYER_MANA_ICON = UI_PLAYER_HEALTH_ICON + 1,
	UI_LINE = UI_PLAYER_MANA_ICON + 1,
	UI_MAP_BUTTON = UI_LINE + 1,
	UI_TUTORIAL = UI_MAP_BUTTON + 1,
	UI_BACKGROUND = UI_TUTORIAL + 1,
	UI_LOOT = UI_BACKGROUND + 1,
	UI_WALL = UI_LOOT + 1,
	UI_MENU = UI_WALL + 1,
	UI_COMBAT_BACKGROUND_1 = UI_MENU + 1,
	UI_COMBAT_BACKGROUND_2 = UI_COMBAT_BACKGROUND_1 + 1,
	UI_COMBAT_BACKGROUND_3 = UI_COMBAT_BACKGROUND_2 + 1,
	TANK = UI_COMBAT_BACKGROUND_3 + 1,
	TANK_PUNCH_0 = TANK + 1,
	TANK_PUNCH_0_1 = TANK_PUNCH_0 + 1,
	TANK_PUNCH_0_2 = TANK_PUNCH_0_1 + 1,
	TANK_PUNCH_1 = TANK_PUNCH_0_2 + 1,
	TANK_PUNCH_1_1 = TANK_PUNCH_1 + 1,
	TANK_PUNCH_1_2 = TANK_PUNCH_1_1 + 1,
	TANK_PUNCH_1_3 = TANK_PUNCH_1_2 + 1,
	TANK_PUNCH_2 = TANK_PUNCH_1_3 + 1,
	TANK_PUNCH_2_1 = TANK_PUNCH_2 + 1,
	TANK_PUNCH_2_2 = TANK_PUNCH_2_1 + 1,
	TANK_PUNCH_2_3 = TANK_PUNCH_2_2 + 1,
	TANK_PUNCH_2_4 = TANK_PUNCH_2_3 + 1,
	TANK_PUNCH_2_5 = TANK_PUNCH_2_4 + 1,
	TANK_PUNCH_2_6 = TANK_PUNCH_2_5 + 1,
	TANK_PUNCH_2_7 = TANK_PUNCH_2_6 + 1,
	TANK_PUNCH_3 = TANK_PUNCH_2_7 + 1,
	TANK_PUNCH_4 = TANK_PUNCH_3 + 1,
	UI_RESTART = TANK_PUNCH_4 + 1,
	UI_EXIT = UI_RESTART + 1,
	TANK_HEAVYPUNCH_0 = UI_EXIT + 1,
	TANK_HEAVYPUNCH_0_1 = TANK_HEAVYPUNCH_0 + 1,
	TANK_HEAVYPUNCH_0_2 = TANK_HEAVYPUNCH_0_1 + 1,
	TANK_HEAVYPUNCH_1 = TANK_HEAVYPUNCH_0_2 + 1,
	TANK_HEAVYPUNCH_1_1 = TANK_HEAVYPUNCH_1 + 1,
	TANK_HEAVYPUNCH_1_2 = TANK_HEAVYPUNCH_1_1 + 1,
	TANK_HEAVYPUNCH_1_3 = TANK_HEAVYPUNCH_1_2 + 1,
	TANK_HEAVYPUNCH_2 = TANK_HEAVYPUNCH_1_3 + 1,
	TANK_HEAVYPUNCH_2_1 = TANK_HEAVYPUNCH_2 + 1,
	TANK_HEAVYPUNCH_2_2 = TANK_HEAVYPUNCH_2_1 + 1,
	TANK_HEAVYPUNCH_2_3 = TANK_HEAVYPUNCH_2_2 + 1,
	TANK_HEAVYPUNCH_2_4 = TANK_HEAVYPUNCH_2_3 + 1,
	TANK_HEAVYPUNCH_2_5 = TANK_HEAVYPUNCH_2_4 + 1,
	TANK_HEAVYPUNCH_2_6 = TANK_HEAVYPUNCH_2_5 + 1,
	TANK_HEAVYPUNCH_2_7 = TANK_HEAVYPUNCH_2_6 + 1,
	TANK_HEAVYPUNCH_3 = TANK_HEAVYPUNCH_2_7 + 1,
	TANK_HEAVYPUNCH_4 = TANK_HEAVYPUNCH_3 + 1,
	HEALER = TANK_HEAVYPUNCH_4 + 1,
	HEALER_HEAL_0 = HEALER + 1,
	HEALER_HEAL_0_1 = HEALER_HEAL_0 + 1,
	HEALER_HEAL_0_2 = HEALER_HEAL_0_1 + 1,
	HEALER_HEAL_1 = HEALER_HEAL_0_2 + 1,
	HEALER_HEAL_1_1 = HEALER_HEAL_1 + 1,
	HEALER_HEAL_1_2 = HEALER_HEAL_1_1 + 1,
	HEALER_HEAL_1_3 = HEALER_HEAL_1_2 + 1,
	HEALER_HEAL_2 = HEALER_HEAL_1_3 + 1,
	HEALER_HEAL_2_1 = HEALER_HEAL_2 + 1,
	HEALER_HEAL_2_2 = HEALER_HEAL_2_1 + 1,
	HEALER_HEAL_2_3 = HEALER_HEAL_2_2 + 1,
	HEALER_HEAL_2_4 = HEALER_HEAL_2_3 + 1,
	HEALER_HEAL_2_5 = HEALER_HEAL_2_4 + 1,
	HEALER_HEAL_2_6 = HEALER_HEAL_2_5 + 1,
	HEALER_HEAL_2_7 = HEALER_HEAL_2_6 + 1,
	HEALER_HEAL_3 = HEALER_HEAL_2_7 + 1,
	HEALER_HEAL_4 = HEALER_HEAL_3 + 1,
	HEALER_SPELL_0 = HEALER_HEAL_4 + 1,
	HEALER_SPELL_0_1 = HEALER_SPELL_0 + 1,
	HEALER_SPELL_0_2 = HEALER_SPELL_0_1 + 1,
	HEALER_SPELL_1 = HEALER_SPELL_0_2 + 1,
	HEALER_SPELL_1_1 = HEALER_SPELL_1 + 1,
	HEALER_SPELL_1_2 = HEALER_SPELL_1_1 + 1,
	HEALER_SPELL_1_3 = HEALER_SPELL_1_2 + 1,
	HEALER_SPELL_2 = HEALER_SPELL_1_3 + 1,
	HEALER_SPELL_2_1 = HEALER_SPELL_2 + 1,
	HEALER_SPELL_2_2 = HEALER_SPELL_2_1 + 1,
	HEALER_SPELL_2_3 = HEALER_SPELL_2_2 + 1,
	HEALER_SPELL_2_4 = HEALER_SPELL_2_3 + 1,
	HEALER_SPELL_2_5 = HEALER_SPELL_2_4 + 1,
	HEALER_SPELL_2_6 = HEALER_SPELL_2_5 + 1,
	HEALER_SPELL_2_7 = HEALER_SPELL_2_6 + 1,
	HEALER_SPELL_3 = HEALER_SPELL_2_7 + 1,
	HEALER_SPELL_4 = HEALER_SPELL_3 + 1,
	RANGED = HEALER_SPELL_4 + 1,
	RANGED_ANIMATION_0 = RANGED + 1,
	RANGED_ANIMATION_0_1 = RANGED_ANIMATION_0 + 1,
	RANGED_ANIMATION_0_2 = RANGED_ANIMATION_0_1 + 1,
	RANGED_ANIMATION_1 = RANGED_ANIMATION_0_2 + 1,
	RANGED_ANIMATION_1_1 = RANGED_ANIMATION_1 + 1,
	RANGED_ANIMATION_1_2 = RANGED_ANIMATION_1_1 + 1,
	ICE_SPEAR = RANGED_ANIMATION_1_2 + 1,
	MELEE = ICE_SPEAR + 1,
	MELEE_PUNCH_0 = MELEE + 1,
	MELEE_PUNCH_0_1 = MELEE_PUNCH_0 + 1,
	MELEE_PUNCH_0_2 = MELEE_PUNCH_0_1 + 1,
	MELEE_PUNCH_1 = MELEE_PUNCH_0_2 + 1,
	MELEE_PUNCH_1_1 = MELEE_PUNCH_1 + 1,
	MELEE_PUNCH_1_2 = MELEE_PUNCH_1_1 + 1,
	MELEE_PUNCH_2 = MELEE_PUNCH_1_2 + 1,
	MELEE_PUNCH_2_1 = MELEE_PUNCH_2 + 1,
	MELEE_PUNCH_2_2 = MELEE_PUNCH_2_1 + 1,
	MELEE_PUNCH_3 = MELEE_PUNCH_2_2 + 1,
	MELEE_PUNCH_3_1 = MELEE_PUNCH_3 + 1,
	MELEE_PUNCH_3_2 = MELEE_PUNCH_3_1 + 1,
	MELEE_SLASH_0 = MELEE_PUNCH_3_2 + 1,
	MELEE_SLASH_0_1 = MELEE_SLASH_0 + 1,
	MELEE_SLASH_0_2 = MELEE_SLASH_0_1 + 1,
	MELEE_SLASH_1 = MELEE_SLASH_0_2 + 1,
	MELEE_SLASH_1_1 = MELEE_SLASH_1 + 1,
	MELEE_SLASH_1_2 = MELEE_SLASH_1_1 + 1,
	MELEE_SLASH_2 = MELEE_SLASH_1_2 + 1,
	MELEE_SLASH_2_1 = MELEE_SLASH_2 + 1,
	MELEE_SLASH_2_2 = MELEE_SLASH_2_1 + 1,
	MELEE_SLASH_3 = MELEE_SLASH_2_2 + 1,
	MELEE_SLASH_3_1 = MELEE_SLASH_3 + 1,
	MELEE_SLASH_3_2 = MELEE_SLASH_3_1 + 1,
	MELEE_HEAVY_HIT_0 = MELEE_SLASH_3_2 + 1,
	MELEE_HEAVY_HIT_0_1 = MELEE_HEAVY_HIT_0 + 1,
	MELEE_HEAVY_HIT_0_2 = MELEE_HEAVY_HIT_0_1 + 1,
	MELEE_HEAVY_HIT_1 = MELEE_HEAVY_HIT_0_2 + 1,
	MELEE_HEAVY_HIT_1_1 = MELEE_HEAVY_HIT_1 + 1,
	MELEE_HEAVY_HIT_1_2 = MELEE_HEAVY_HIT_1_1 + 1,
	MELEE_HEAVY_HIT_2 = MELEE_HEAVY_HIT_1_2 + 1,
	MELEE_HEAVY_HIT_2_1 = MELEE_HEAVY_HIT_2 + 1,
	MELEE_HEAVY_HIT_2_2 = MELEE_HEAVY_HIT_2_1 + 1,
	TEXTURE_COUNT = MELEE_HEAVY_HIT_2_2 + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	EGG = COLOURED + 1,
	SALMON = EGG + 1,
	TEXTURED = SALMON + 1,
	WATER = TEXTURED + 1,
	FIRE = WATER + 1,
	EFFECT_COUNT = FIRE + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	EGG = SPRITE + 1,
	DEBUG_LINE = EGG + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

