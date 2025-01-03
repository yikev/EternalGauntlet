#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "game_states.hpp"
#include "combat_stages.hpp"
#include "decision_tree.hpp"

// // KEVIN: combat system
// enum class TurnState {
//     PLAYER_TURN,
//     ENEMY_TURN,
// };

// // KEVIN: game flow
// enum class GameState {
//     PLAYING,
//     VICTORY,
// 	MAP
// };

// ROY: UI
const int UI_ATTACK_BUTTON_Y = window_height_px - 110;
const int UI_ATTACK_BUTTON_1_X = window_width_px/2 - 5 - 75 - 150 - 10;
const int UI_ATTACK_BUTTON_2_X = window_width_px/2 - 5 - 75;
const int UI_ATTACK_BUTTON_3_X = window_width_px/2 + 5 + 75;
const int UI_ATTACK_BUTTON_4_X = window_width_px/2 + 5 + 75 + 150 + 10;

// SKYE: MAP UI


const int UI_MAP_BUTTON_Y = 600;
const int UI_MAP_BUTTON_1_X = window_width_px / 2 - 200;
const int UI_MAP_BUTTON_2_X = window_width_px / 2;
const int UI_MAP_BUTTON_3_X = window_width_px / 2 + 200;

// KEVIN: LOOT UI

const int UI_LOOT_BUTTON_Y = 400;
const int UI_LOOT_BUTTON_X = window_width_px / 2 - 200;

// JAIVEER: MENU AI
const int UI_MENU_BUTTON_1_X = window_width_px / 2 - 150;
const int UI_MENU_BUTTON_1_Y = 700;
const int UI_MENU_BUTTON_2_X = window_width_px / 2 + 150;
const int UI_MENU_BUTTON_2_Y = 700;
const int UI_MENU_BUTTON_3_X = window_width_px / 2 - 150;
const int UI_MENU_BUTTON_3_Y = 500;
const int UI_MENU_BUTTON_4_X = window_width_px / 2 + 150;
const int UI_MENU_BUTTON_4_Y = 500;

class RenderSystem;
// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();
	
	void CalculateFrameRate();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	Entity getPlayerEntity();

	Map generate(int numLevels);

	void printMap() const;

	void loadCombatStage(int stageIndex);

	void initialize_combat(int selected_location);

	void updateAttackButtonText();

	GameState getCurrentGameState() const;

	void setBackground(Entity back, int i);

	void setBackground(int back);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);
	void clearWalls();
	// Check for collisions
	void handle_collisions();

	void removeEntity(Entity entity);

	static std::vector<LootOption> currentLootOptions;

	// Should the game be over ?
	bool is_over()const;

	// Healer AI
	bool canAttackKillPlayer(Entity enemy, Entity player);
	int getBestAttackIndex(const std::vector<Attack>& attacks);
    Attack getHealingAttack(const std::vector<Attack>& attacks);
    void applyHeal(Entity healer, Entity target, const Attack& healAttack);
    Entity findLowestHpAlly(Entity healer);
    void executeHealerTurn(Entity healer, Entity player);
	void enemyAttack(int attack_id, Entity enemyEntity);
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_click(GLFWwindow* window, int button, int action, int mods);

	// restart level
	void restart_game();
	void reset_game();
	// float xScale = 1.0f;
    // float yScale = 1.0f;

	// KEVIN: combat system attempt
	void applyDamageSystem();
	void nextTurn();    
    void enemyTurn();
    void playerAttack(int attack_id, Entity enemy); 
	void addNewAttackToHero(Attack newAttack, int index);
	void finishLoot();
	void resetPlayerAttacks();
	std::vector<CombatStage> combatStages;
	int currentStage = 0;
	void loadGame();
	void saveGame(const PlayerSaveData& data, const std::string& filename);

	// OpenGL window handle
	GLFWwindow* window;

	// Number of HERO eaten by the salmon, displayed in the window title
	unsigned int points;

	// Game state
	RenderSystem* renderer;
	float current_speed;
	float next_eel_spawn;
	float next_fish_spawn;
	Entity player_salmon;

	// KEVIN: added hero and enemy
	// Entity hero;
    Entity meleeEnemy;	// temporary
	// Entity rangedEnemy; // temporary
	bool hero_dead = false;
	float fade_timer = 0.f;
	float hit_timer = 0.f;
	float parry_timer = 0.f;
	Map map;
	Level current_Level;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* hero_hit_sound;
	Mix_Chunk* hero_death_sound;
	Mix_Chunk* enemy_hit_sound;
	Mix_Chunk* loot_sound;
	Mix_Chunk* enemy_death_sound;
	Mix_Chunk* fireball_sound;
	Mix_Chunk* heal_sound;
	Mix_Chunk* woosh_sound;
	Mix_Chunk* punch_sound;
	Mix_Chunk* arrow_sound;
	Mix_Chunk* blade_sound;
	Mix_Chunk* lightning_sound;
	Mix_Chunk* zap_sound;
	Mix_Chunk* quake_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// KEVIN: combat system
	TurnState current_turn = TurnState::PLAYER_TURN;

	// KEVIN: game state
	GameState current_game_state = GameState::MENU;
	bool user_selected_combat_location = false;
	bool user_selected_map = false;
	bool user_selected_loot = false;
	bool user_selected_tutorial = false;

	bool xchange = false;
	bool ychange = false;

	bool input_disabled = false;
	int attack_to_use = 0;
	int enemy_attack_to_use = 0;
	int enemies_remaining_turns;
	int fps;
	bool removedAttack = false;
};
