// Header
#include "world_system.hpp"
#include "render_system.hpp"
#include "world_init.hpp"
#include "enemy_attacks.hpp"
#include "combat_stages.hpp"
#include "loot.hpp"
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include <iostream>
#include "world_init.hpp"
#include <glm/trigonometric.hpp>

#ifdef __unix__         
...
#elif defined(_WIN32) || defined(WIN32) 

#define OS_Windows

#endif


// Game configuration
const size_t MAX_NUM_EELS = 15;
const size_t MAX_NUM_FISH = 5;
const size_t EEL_SPAWN_DELAY_MS = 2000 * 3;
int mapCounter;
bool game_started = false;
bool isChoosingTarget = false;
Attack dummyAttack = {"None", 0, 0, 0, 0, 0, false};
Attack& selectedAttack = dummyAttack;
Entity hero;
std::vector<LootOption> WorldSystem::currentLootOptions;
const LootOption* selectedLootOption = nullptr;
int selectedLootIndex = -1;
std::vector<Entity> attackButtonTextEntities;
bool user_chose_attack = false;
static float delay_timer = 0.0f;
static bool waiting = false; 
bool playerAttackInProgress = false;
extern std::unordered_set<std::string> chosenAttacks;

// create the underwater world
WorldSystem::WorldSystem()
	: points(0)
	, next_eel_spawn(0.f)
	, next_fish_spawn(0.f) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

void WorldSystem::CalculateFrameRate()
{
	static float framesPerSecond = 0.0f;       //  Stores FPS
	static float lastTime = 0.0f;       // Time from last frame
	float currentTime;

#ifdef OS_Windows
	/* Windows code */
	currentTime = GetTickCount64() * 0.001f;

#else
	/* GNU/Linux code */
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	currentTime = 0.001f * (uint64_t)(ts.tv_nsec / 1000000) + ((uint64_t)ts.tv_sec * 1000ull);

#endif    

	
	++framesPerSecond;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;

		this->fps = (int)framesPerSecond;
		registry.fpss.components[0].fps = this->fps;
		// fprintf(stderr, "\nCurrent Frames Per Second: %d\n\n", this->fps);
		framesPerSecond = 0;
	}
}

WorldSystem::~WorldSystem() {
	
	// destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (hero_hit_sound != nullptr)
		Mix_FreeChunk(hero_hit_sound);
	if (hero_death_sound != nullptr)
		Mix_FreeChunk(hero_death_sound);
	if (enemy_hit_sound != nullptr)
		Mix_FreeChunk(enemy_hit_sound);
	if (loot_sound != nullptr)
		Mix_FreeChunk(loot_sound);
	if (fireball_sound != nullptr)
		Mix_FreeChunk(fireball_sound);
	if (heal_sound != nullptr)
		Mix_FreeChunk(heal_sound);
	if (woosh_sound != nullptr)
		Mix_FreeChunk(woosh_sound);
	if (punch_sound != nullptr)
		Mix_FreeChunk(punch_sound);
	if (arrow_sound != nullptr)
		Mix_FreeChunk(arrow_sound);
	if (blade_sound != nullptr)
		Mix_FreeChunk(blade_sound);
	if (lightning_sound != nullptr)
		Mix_FreeChunk(lightning_sound);
	if (zap_sound != nullptr)
		Mix_FreeChunk(zap_sound);
	if (quake_sound != nullptr)
		Mix_FreeChunk(quake_sound);

	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}


// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	// window = glfwCreateWindow(window_width_px, window_height_px, "Eternal Gauntlet", glfwGetPrimaryMonitor(), nullptr);
	// Windowed mode
	 window = glfwCreateWindow(window_width_px, window_height_px, "Eternal Gauntlet", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// int framebufferWidth, framebufferHeight;
    // glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    // Calculate scaling factors
    // xScale = static_cast<float>(framebufferWidth) / window_width_px;
    // yScale = static_cast<float>(framebufferHeight) / window_height_px;

    // printf("Framebuffer scaling factors: xScale = %.2f, yScale = %.2f\n", xScale, yScale);

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_click(wnd, _0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	hero_hit_sound = Mix_LoadWAV(audio_path("oof.wav").c_str());
	hero_death_sound = Mix_LoadWAV(audio_path("amongus.wav").c_str());
	enemy_hit_sound = Mix_LoadWAV(audio_path("salmon_death_sound.wav").c_str());
	loot_sound = Mix_LoadWAV(audio_path("loot_sound.wav").c_str());
	enemy_death_sound = Mix_LoadWAV(audio_path("enemy_death.wav").c_str());
	fireball_sound = Mix_LoadWAV(audio_path("fireball.wav").c_str());
	heal_sound = Mix_LoadWAV(audio_path("heal.wav").c_str());
	woosh_sound = Mix_LoadWAV(audio_path("woosh.wav").c_str());
	punch_sound = Mix_LoadWAV(audio_path("punch.wav").c_str());
	arrow_sound = Mix_LoadWAV(audio_path("arrow.wav").c_str());
	blade_sound = Mix_LoadWAV(audio_path("blade.wav").c_str());
	lightning_sound = Mix_LoadWAV(audio_path("lightning.wav").c_str());
	zap_sound = Mix_LoadWAV(audio_path("zap.wav").c_str());
	quake_sound = Mix_LoadWAV(audio_path("quake.wav").c_str());

	if (background_music == nullptr || hero_hit_sound == nullptr || hero_death_sound == nullptr || enemy_hit_sound == nullptr || loot_sound == nullptr || enemy_death_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("death_sound.wav").c_str(),
			audio_path("eat_sound.wav").c_str(),
			audio_path("enemy_hit_sound.wav").c_str());
		return nullptr;
	}

	return window;
}
Entity WorldSystem::getPlayerEntity() {
	return hero;
}

void WorldSystem::removeEntity(Entity entity) {
    // Ensure the entity is removed from all component registries
    if (registry.renderRequests.has(entity)) {
        registry.renderRequests.remove(entity);
    }
    if (registry.motions.has(entity)) {
        registry.motions.remove(entity);
    }
    if (registry.healths.has(entity)) {
        registry.healths.remove(entity);
    }
    if (registry.enemies.has(entity)) {
        registry.enemies.remove(entity);
    }
    if (registry.enemyTypes.has(entity)) {
        registry.enemyTypes.remove(entity);
    }
    if (registry.hits.has(entity)) {
        registry.hits.remove(entity);
    }
    if (registry.attacks.has(entity)) {
        registry.attacks.remove(entity);
    }
    if (registry.meshPtrs.has(entity)) {
        registry.meshPtrs.remove(entity);
    }
    // Finally, remove all components of the entity
    registry.remove_all_components_of(entity);
}

GameState WorldSystem::getCurrentGameState() const {
    return current_game_state;
}

void WorldSystem::setBackground(Entity back, int i) {
	if(i == 1)
		registry.renderRequests.get(back).used_texture = TEXTURE_ASSET_ID::UI_COMBAT_BACKGROUND_1;
	if(i == 2)
		registry.renderRequests.get(back).used_texture = TEXTURE_ASSET_ID::UI_COMBAT_BACKGROUND_2;
	if(i == 3)
		registry.renderRequests.get(back).used_texture = TEXTURE_ASSET_ID::UI_COMBAT_BACKGROUND_3;
}

void WorldSystem::loadCombatStage(int stageIndex) {
	Entity entity = registry.players.entities[0];
	Motion& motion = registry.motions.get(entity);
	motion.velocity = { 0 , 0 };
	motion.position = { window_width_px * 0.2, window_height_px / 2 };

    const std::vector<CombatStage>& combatStages = getCombatStages();

    const CombatStage& stage = combatStages[stageIndex];

    // Clear existing enemies from previous stages
    // for (Entity enemy : registry.enemies.entities) {
    //     removeEntity(enemy);
    // }

    // Spawn enemies for this stage
    for (const EnemyData& enemyData : stage.enemies) {
		Entity enemy;
		if (stageIndex == 2) {
			enemy = createEnemy(renderer, { (window_width_px)-enemyData.position.x / 2, window_height_px / 2 - enemyData.position.y }, enemyData.type);
		}
		else if (stageIndex == 3) {
			enemy = createEnemy(renderer, { (window_width_px)-enemyData.position.x / 2, window_height_px * 0.4f - enemyData.position.y }, enemyData.type);
		}
		else if (stageIndex == 1) {
			enemy = createEnemy(renderer, { (window_width_px)-enemyData.position.x / 2, window_height_px / 2 - enemyData.position.y }, enemyData.type);
		}
		else if (stageIndex == 4) {
			enemy = createEnemy(renderer, { (window_width_px)-enemyData.position.x / 2, window_height_px / 2 - enemyData.position.y }, enemyData.type);
		}
		else if (stageIndex == 5) {
			enemy = createEnemy(renderer, { (window_width_px)-enemyData.position.x / 2, window_height_px / 2 - enemyData.position.y }, enemyData.type);
		}
		else {
			enemy = createEnemy(renderer, { (window_width_px)-enemyData.position.x / 2, window_height_px / 2 - enemyData.position.y }, enemyData.type);
		}
		registry.colors.insert(enemy, {1.f, 1.f, 1.f});
		//registry.healths.emplace_with_duplicates(enemy, Health{100, 100});
		if(mapCounter <= 3) {
			registry.healths.get(enemy).max_hp /= 2;
			registry.healths.get(enemy).hp /= 2;
		}else if(mapCounter <= 5) {
			registry.healths.get(enemy).max_hp *= 0.6;
			registry.healths.get(enemy).hp *= 0.6;
		}else if(mapCounter <= 7) {
			registry.healths.get(enemy).max_hp *= 0.7;
			registry.healths.get(enemy).hp *= 0.7;
		}else if(mapCounter <= 9) {
			registry.healths.get(enemy).max_hp *= 0.9;
			registry.healths.get(enemy).hp *= 0.9;
		}

    }

    currentStage = stageIndex;  
    enemies_remaining_turns = registry.enemies.entities.size(); 

    std::cout << "Loaded Combat Stage " << stageIndex << " with " << stage.enemies.size() << " enemies." << std::endl;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	mapCounter = 0;
	this->renderer = renderer_arg;

    combatStages = getCombatStages();
    current_game_state = GameState::TUTORIAL;
    map = generate(10);
    // Playing background music indefinitely
    Mix_PlayMusic(background_music, -1);
    fprintf(stderr, "Loaded music\n");

    this->map = generate(10);
    this->current_Level = this->map.levels.begin()->second;

    // SKYE: Create 3 UI map buttons
    createUIMapButton(renderer, { window_width_px / 2 - 200, 600 });
    createUIMapButton(renderer, { window_width_px / 2, 600 });
    createUIMapButton(renderer, { window_width_px / 2 + 200, 600 });

    // Loot Reward Confirm Button.
    createUILootButton(renderer, { window_width_px / 2 - 200, 400 });
    
    createUITutorial(renderer, { window_width_px / 2, 550 });

    createUIBackground(renderer, {window_width_px/2, window_height_px/2});

	Entity m1 = createUIMenuButton(renderer, {UI_MENU_BUTTON_1_X, UI_MENU_BUTTON_1_Y}, { UI_MAP_BUTTON_WIDTH, UI_MAP_BUTTON_HEIGHT });
	Entity m2 = createUIMenuButton(renderer, {UI_MENU_BUTTON_2_X, UI_MENU_BUTTON_2_Y}, { UI_MAP_BUTTON_WIDTH, UI_MAP_BUTTON_HEIGHT });
	Entity m3 = createUIMenuButton(renderer, {UI_MENU_BUTTON_3_X, UI_MENU_BUTTON_3_Y}, { UI_MAP_BUTTON_WIDTH, UI_MAP_BUTTON_HEIGHT });
	Entity m4 = createUIMenuButton(renderer, {UI_MENU_BUTTON_4_X, UI_MENU_BUTTON_4_Y}, { UI_MAP_BUTTON_WIDTH/10000, UI_MAP_BUTTON_HEIGHT/10000 });

	registry.renderRequests.get(m1).used_texture = TEXTURE_ASSET_ID::UI_RESTART;
	registry.renderRequests.get(m2).used_texture = TEXTURE_ASSET_ID::UI_EXIT;

	registry.uis.get(m1).scale.x += 20;
	registry.uis.get(m1).scale.y += 20;

	Entity back2 = createUIBackground(renderer, { window_width_px / 2, window_height_px / 2 });
    //registry.uis.get(back2).scale *= 2;
	setBackground(back2, 3);

	current_game_state = GameState::MENU;
    renderer->setGameState(GameState::MENU);
    
    hero = createHero(renderer, { window_width_px * 0.1, window_height_px * 0.9 });
    registry.colors.insert(hero, {1.f, 1.f, 1.f});
    // registry.healths.emplace_with_duplicates(hero, Health{100});


    // Set all states to default
    restart_game();
}


void WorldSystem::printMap() const {
    for (const auto& pair : map.levels) {
        const Level& level = pair.second;
        std::cout << "Level " << level.number << " connections: ";
        for (int conn : level.connections) {
            std::cout << conn << " ";
        }
        std::cout << std::endl;
    }
}

Entity dead_enemy = 0; 
const float TEXTURE_CHANGE_TIMER = 1000.f;  


// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Points: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());
	CalculateFrameRate();

	// Delay enemy turns
	delay_timer += elapsed_ms_since_last_update;
    if (delay_timer >= 2000.0f) { // 2-second delay
        waiting = false;
        delay_timer = 0.0f;
    }

	if (current_game_state == GameState::PLAYING) {
		Motion& motion = registry.motions.get(hero);
		motion.velocity = { 0 , 0 };
		motion.position = { window_width_px * 0.2, window_height_px / 2 };
	}

	if (current_game_state == GameState::MAP) {

		Motion& motion = registry.motions.get(hero);

		float min_x = 0;
		float max_x = window_width_px;
		float min_y = 0;
		float max_y = window_height_px;

		float half_width = motion.scale.x / 2.0f;
		float half_height = motion.scale.y / 2.0f;

		if (motion.position.x - half_width < min_x) {
			motion.position.x = min_x + half_width;
			motion.velocity.x = 0;                
		}
		if (motion.position.x + half_width > max_x) {
			motion.position.x = max_x - half_width;
			motion.velocity.x = 0;
		}
		if (motion.position.y - half_height < min_y) {
			motion.position.y = min_y + half_height;
			motion.velocity.y = 0;
		}
		if (motion.position.y + half_height > max_y) {
			motion.position.y = max_y - half_height;
			motion.velocity.y = 0;
		}
        // Kevin: currently generates a blank screen
		renderer->renderMap();
        if (user_selected_combat_location) { 
            // We swap to combat
			motion.velocity.x = 0;
			motion.velocity.y = 0;
			renderer->setGameState(GameState::PLAYING); //this renders combat
            current_game_state = GameState::PLAYING;
			motion.velocity.x = 0;
			motion.velocity.y = 0;
			// registry.list_all_components();
        }

		if (registry.mapEnemies.size() == 0) {
			Motion& motion = registry.motions.get(hero);
			const float wall_length = window_width_px / 6.0f;
			const float wall_thickness = window_width_px / 20.0f;
			if (mapCounter == 0) {
				clearWalls();
				motion.position = { window_width_px * 0.2, window_height_px / 2 };
				createMapEnemy(renderer, { window_width_px * 0.8, window_height_px / 2 }, 2);
				createTutorialText();
				mapCounter++;
			}
			else if (mapCounter == 1) {

				deleteTutorialText();
				setBackground(registry.uis.entities[10], 1);
				clearWalls();
				motion.position = { window_width_px * 0.2, window_height_px / 1.5 };
				//createMapWall(renderer, { 0.1, 0.4 }, { .3, .1 });
				createMapWall(renderer, { 0.75, 0.8 }, { .05, .4 });
				createMapWall(renderer, { 0.75, 0.1 }, { .05, .4 });
				createMapEnemy(renderer, { window_width_px * 0.1, window_height_px * 0.15 }, 0);
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.85 }, 1);
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.15 }, 4);
				mapCounter++;
			}
			else if (mapCounter == 2) {
				setBackground(registry.uis.entities[10], 2);
				clearWalls();
				motion.position = { window_width_px * 0.2, window_height_px / 2 };
				createMapWall(renderer, { 0.9, 0.6 }, { .3, .1 });
				createMapWall(renderer, { 0.5, 0.8 }, { .05, .4 });
				createMapWall(renderer, { 0.75, 0.1 }, { .05, .4 });
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.9 }, 0);
				createMapEnemy(renderer, { window_width_px * 0.7, window_height_px / 2 }, 5);
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.1 }, 2);
				mapCounter++;
			}
			else if (mapCounter == 3) {
				setBackground(registry.uis.entities[10], 3);
				clearWalls();
				motion.position = { window_width_px * 0.2, window_height_px / 2 };
				createMapWall(renderer, { 0.55, 0.5 }, { .05, .4 });
				createMapWall(renderer, { 0.75, 0.8 }, { .05, .4 });
				createMapWall(renderer, { 0.75, 0.1 }, { .05, .4 });
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.1 }, 4);
				mapCounter++;
			}
			else if (mapCounter == 4) {
				clearWalls();
				//createMapWall(renderer, { 0.85, 0.6 }, { .4, .05 });
				createMapWall(renderer, { 0.5, 0.8 }, { .05, .4 });
				createMapWall(renderer, { 0.65, 0.25 }, { .3, .05 });
				motion.position = { window_width_px * 0.2, window_height_px / 2 };
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.9 }, 2);
				mapCounter++;
			}
			else if (mapCounter == 5) {
				clearWalls();
				motion.position = { window_width_px * 0.5, window_height_px * 0.1 };
				//createMapWall(renderer, { 0.25, 0.4 }, { .05, .8 });
				createMapWall(renderer, { 0.5, 0.8 }, { .05, .4 });
				createMapWall(renderer, { 0.75, 0.1 }, { .05, .4 });
				createMapEnemy(renderer, { window_width_px * 0.1, window_height_px * 0.1 }, 4);
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.9 }, 1);
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.1 }, 5);
				mapCounter++;
			}
			else if (mapCounter == 6) {
				clearWalls();
				createMapWall(renderer, { 0.45, 0.4 }, { .4, .05 });
				createMapWall(renderer, { 0.75, 0.8 }, { .05, .4 });
				createMapWall(renderer, { 0.75, 0.1 }, { .05, .4 });
				motion.position = { window_width_px * 0.2, window_height_px / 2 };
				createMapEnemy(renderer, { window_width_px * 0.6, window_height_px * 0.9 }, 0);
				createMapEnemy(renderer, { window_width_px * 0.5, window_height_px / 10 }, 1);
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.1 }, 4);
				mapCounter++;
			}
			else if (mapCounter == 7) {
				clearWalls();
				motion.position = { window_width_px * 0.5, window_height_px * 0.1 };
				createMapWall(renderer, { 0.25, 0.8 }, { .05, .4 });
				//createMapWall(renderer, { 0.5, 0.3 }, { .6, .05 });
				createMapWall(renderer, { 0.75, 0.8 }, { .05, .4 });
				createMapEnemy(renderer, { window_width_px * 0.85, window_height_px * 0.9 }, 0);
				createMapEnemy(renderer, { window_width_px * 0.95, window_height_px * 0.9 }, 1);
				createMapEnemy(renderer, { window_width_px * 0.1, window_height_px * 0.9 }, 2);
				mapCounter++;
			}
			else if (mapCounter == 8) {
				clearWalls();
				motion.position = { window_width_px * 0.5, window_height_px * 0.9 };
				createMapWall(renderer, { 0.25, 0.8 }, { .05, .4 });
				//createMapWall(renderer, { 0.5, 0.3 }, { .6, .05 });
				createMapWall(renderer, { 0.75, 0.8 }, { .05, .4 });
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.1 }, 1);
				createMapEnemy(renderer, { window_width_px * 0.1, window_height_px * 0.1 }, 5);
				mapCounter++;
			}
			else if (mapCounter == 9) {
				clearWalls();
				createMapWall(renderer, { 0.25, 0.2 }, { .05, .4 });
				createMapWall(renderer, { 0.25, 0.8 }, { .05, .4 });
				createMapWall(renderer, { 0.75, 0.4 }, { .05, .8 });
				motion.position = { window_width_px * 0.1, window_height_px / 2 };
				createMapEnemy(renderer, { window_width_px * 0.5, window_height_px * 0.1 }, 1);
				createMapEnemy(renderer, { window_width_px * 0.9, window_height_px * 0.1 }, 4);
				createMapEnemy(renderer, { window_width_px * 0.1, window_height_px * 0.1 }, 2);
				mapCounter++;
			}
			else if (mapCounter == 10) {
				clearWalls();
				motion.position = { window_width_px * 0.2, window_height_px * 0.5 };
				createMapEnemy(renderer, { window_width_px * 0.7, window_height_px * 0.5 }, 3);
				//createMapWall(renderer, { 0.5, 0.05 }, { .95, .1 });
				//createMapWall(renderer, { 0.5, 0.95 }, { .95, .1 });
				mapCounter++;
			}
		}
        return true;
    }

	if (current_game_state == GameState::TUTORIAL) {
		// Kevin: currently generates a blank screen
		renderer->renderTutorial();
		if (user_selected_map) {
			// We swap to combat
			renderer->setGameState(GameState::MAP); //this renders map
			current_game_state = GameState::MAP;
		}
		return true;
	}

	if (current_game_state == GameState::MENU) {
		renderer->renderMenu();
		renderer->setGameState(GameState::MENU);
		current_game_state = GameState::MENU;
		if (user_selected_map) {
			// We swap to combat
			renderer->setGameState(GameState::MAP);
			current_game_state = GameState::MAP;
		} else if (user_selected_tutorial) {
			// We swap to combat
			renderer->setGameState(GameState::TUTORIAL); 
			current_game_state = GameState::TUTORIAL;
		}
		return true;
	}

	if (current_game_state == GameState::VICTORY) {
		printf("Size of attackButtonTextEntities: %zu\n", attackButtonTextEntities.size());
		printf("Restarting game...\n");
		current_turn = TurnState::PLAYER_TURN;
		current_game_state = GameState::LOOT; //testing map
		renderer->setGameState(GameState::LOOT);
		Health& health = registry.healths.get(hero);
		health.hp = health.max_hp;
		WorldSystem::currentLootOptions = getAvailableLootOptions();
		ScreenState& screen = registry.screenStates.components[0];
		restart_game();
		// Player gets full mana, change if needed
		Mana& heroMana = registry.manas.get(hero);
        heroMana.currentMana = heroMana.maxMana; 
		// current_game_state = GameState::TUTORIAL;

		std::cout << "Entities with Health component:" << std::endl;

    for (Entity entity : registry.healths.entities) {
        if (!registry.healths.has(entity)) {
            std::cerr << "Warning: Skipping invalid entity ID "
                      << static_cast<unsigned int>(entity) << std::endl;
            continue;
        }

        const Health& health = registry.healths.get(entity);
        std::cout << "Entity ID: " << static_cast<unsigned int>(entity)
                  << ", HP: " << health.hp 
                  << ", Max HP: " << health.max_hp << std::endl;
    }

		return true;
	}

	if (current_game_state == GameState::PLAYING) {

		if (dead_enemy != 0) {
			Motion& motion = registry.motions.get(dead_enemy);  // The character's color
			input_disabled = true;
			fade_timer += elapsed_ms_since_last_update;

			// Define start and end colors
			float start_color = 0.f;
			float end_color = 1;

			float duration = 3000.f;

			float t = std::min(fade_timer / duration, 1.0f);

			motion.angle = (1 - t) * start_color + t * end_color;

			if (fade_timer >= duration) {
				// Perform the action after the duration is complete
				// For example, remove the enemy from the game

				bool all_enemies_dead = true;  // Assume all enemies are dead initially
				for (Entity e : registry.enemies.entities) {
					if (registry.healths.has(e) && registry.healths.get(e).hp > 0) {
						all_enemies_dead = false;  // Found a living enemy, so not all are dead
						break;
					}
				}

				if (all_enemies_dead) {
					removeEntity(dead_enemy);
					printf("All enemies defeated! Victory!\n");
					current_game_state = GameState::VICTORY;  // Trigger victory state'
				} else {
					removeEntity(dead_enemy); // currently not removing the final enemy.
				}

				// Reset the dead_enemy and fade_timer to avoid re-triggering
				dead_enemy = 0;  // Mark dead_enemy as "no longer valid"
				fade_timer = 0.0f;  // Reset the fade timer
				input_disabled = false;
			}

			return true;
		}

		CalculateFrameRate();

	// KEVIN: Player death check
		if (hero_dead) {
			Mix_PlayChannel(-1, hero_death_sound, 0);
			fade_timer += elapsed_ms_since_last_update;

			// Gradually increase the darken_screen_factor
			ScreenState& screen = registry.screenStates.components[0];

			float start_factor = 0.0f;  // Starting factor (no darkness)
			float end_factor = 1.0f;

			float t = min(fade_timer / 3000.f, 1.0f);
			screen.darken_screen_factor = (1 - t) * start_factor + t * end_factor;

			// "Kill" the player by changing them into a zombie
			Entity player = registry.players.entities[0];
			registry.renderRequests.get(player).used_texture = TEXTURE_ASSET_ID::DEAD;
			registry.motions.get(player).angle = glm::radians(270.0);

			// Once fully black, restart the game
			if (fade_timer >= 3000.f) {
				screen.darken_screen_factor = 0;
				current_turn = TurnState::PLAYER_TURN;
				registry.renderRequests.get(player).used_texture = TEXTURE_ASSET_ID::HERO_IDLE_0;
				registry.motions.get(player).angle = glm::radians(0.0);
				// reset_game();
				// hero_dead = false;
				renderer->game_state = GameState::GAMEOVER;
			}

			return true;
		}

		if (registry.hits.get(registry.players.entities[0]).hit == true) {
			hit_timer += elapsed_ms_since_last_update;
			if (hit_timer >= 1000.f) {
				registry.hits.get(registry.players.entities[0]).hit = false;
				hit_timer = 0.0;
			}
		}

		for (Entity enemy : registry.enemies.entities) {
			if (registry.hits.get(enemy).hit == true) {
				hit_timer += elapsed_ms_since_last_update;
				if (hit_timer >= 100.f) {
					registry.hits.get(enemy).hit = false;
					hit_timer = 0.0;
				}
			}
		}

	if (registry.parrys.has(hero) || !registry.parryCooldowns.has(hero)) {
		if (!registry.parryCooldowns.has(hero)) {
			registry.parryCooldowns.emplace(hero);
		}
		parry_timer += elapsed_ms_since_last_update;
		if (parry_timer >= 750.f) {
			if (registry.parrys.has(hero)) {
				registry.parrys.remove(hero);
				std::cout << "Hero has ended parrying" << std::endl;
			}
			parry_timer = 0.0;
		}

			if (parry_timer >= 3000.f) {
				if (registry.parryCooldowns.has(hero)) {
					registry.parryCooldowns.remove(hero);
					std::cout << "Hero's parry cooldown has ended" << std::endl;
				}
				parry_timer = 0.0;
			}
		}

		auto& motions_registry = registry.motions;

		// Remove entities that leave the screen on the left side
		// Iterate backwards to be able to remove without unterfering with the next object to visit
		// (the containers exchange the last element with the current)
		for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i) {
			Motion& motion = motions_registry.components[i];
			if (motion.position.x + abs(motion.scale.x) < 0.f) {
				if (!registry.players.has(motions_registry.entities[i])) // don't remove the player
					registry.remove_all_components_of(motions_registry.entities[i]);
			}
		}

		applyDamageSystem();

		// KEVIN: combat system
		if (current_turn == TurnState::PLAYER_TURN) {
			// Wait for player input
		} else if (current_turn == TurnState::ENEMY_TURN) {
			enemyTurn();
		}

		// Processing the salmon state
		assert(registry.screenStates.components.size() <= 1);
		ScreenState &screen = registry.screenStates.components[0];

		float min_counter_ms = 3000.f;

		// reduce window brightness if the salmon is dying
		screen.darken_screen_factor = 1 - min_counter_ms / 3000;
	}


	return true;
}

// JAIVEER: map generation
Map WorldSystem::generate(int numLevels) {
   	Map map;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, numLevels - 1);

    // Create nodes
    for (int i = 0; i < numLevels; ++i) {
        Level level;
        level.number = i;
		level.enemies = 2;
        level.type = "room"; 
        map.levels[level.number] = level;
        }

    // Make sure there is a path to the last node
    for (int i = 0; i < numLevels - 1; ++i) {
        map.levels[i].connections.push_back(i + 1);
        }
	 
    // Add additional random connections
    for (int i = 0; i < numLevels; ++i) {
        int numConnections = dis(gen) % 3 + 1; // Each node will have 1 to 3 connections
        for (int j = 0; j < numConnections; ++j) {
            int target = dis(gen);
            if (target != i && std::find(map.levels[i].connections.begin(), map.levels[i].connections.end(), target) == map.levels[i].connections.end()) {
                map.levels[i].connections.push_back(target);
            }
        }
    }

	// Remove backward connections
    for (int i = 0; i < numLevels; ++i) {
        map.levels[i].connections.erase(
        std::remove_if(
            map.levels[i].connections.begin(),
            map.levels[i].connections.end(),
            [i](int target) {
            return target < i;
            }
        ),
        map.levels[i].connections.end()
        );
    }
    return map;
}

void WorldSystem::reset_game() {
    mapCounter = 0;
    Mana& heroMana = registry.manas.get(hero);
    heroMana.currentMana = 100;
    heroMana.maxMana = 100;
	renderer->noMana = false;
	for (Entity enemy : registry.enemies.entities) {
        removeEntity(enemy);
    }
	for (Entity e : registry.walls.entities) {
		if (registry.walls.has(e)) {
			registry.remove_all_components_of(e);
		}
	}
    registry.enemies.entities.clear(); 
    resetPlayerAttacks();
	playerAttackInProgress = false;
	user_chose_attack = false;
	chosenAttacks.clear();
    restart_game();
    // hero = createHero(renderer, { window_width_px * 0.2, window_height_px / 2});
    Health& heroHealth = registry.healths.get(hero);
    heroHealth.max_hp = 150;
    heroHealth.hp = 150;
    current_game_state = GameState::MENU;
    renderer->game_state = GameState::MENU;

    // Resolves memory leaks. Remove all UITEXTs
    for (Entity e : registry.mapEnemies.entities) {
        if (registry.mapEnemies.has(e)) {
            registry.remove_all_components_of(e);
        }
    }

    if (registry.motions.has(hero)) {
        Motion& motion = registry.motions.get(hero);
        motion.position = { window_width_px * 0.1, window_height_px * 0.9 };
        motion.angle = 0;
    }
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	//mapCounter = 0;
    // Debugging for memory/component leaks
    // registry.list_all_components();
    printf("Restarting\n");
    
    attackButtonTextEntities.clear();
    
    // Resolves memory leaks. Remove all UITEXTs
    for (Entity e : registry.texts.entities) {
        if (registry.texts.has(e)) {
            registry.remove_all_components_of(e);
        }
    }

	for (Entity e : registry.combatTutorialTexts.entities) {
		if (registry.combatTutorialTexts.has(e)) {
			registry.remove_all_components_of(e);
		}
	}

	for (Entity e : registry.mapTutorialTexts.entities) {
		if (registry.mapTutorialTexts.has(e)) {
			registry.remove_all_components_of(e);
		}
	}

    // while (registry.motions.entities.size() > 0)
    //     registry.remove_all_components_of(registry.motions.entities.back());

    // ROY: init UI (after hero/enemies are created)
    Entity attackButton1 = createUIAttackButton(renderer, { UI_ATTACK_BUTTON_1_X, UI_ATTACK_BUTTON_Y }, 0, window_width_px, window_height_px);
    Entity attackButton2 = createUIAttackButton(renderer, { UI_ATTACK_BUTTON_2_X, UI_ATTACK_BUTTON_Y }, 1, window_width_px, window_height_px);
    Entity attackButton3 = createUIAttackButton(renderer, { UI_ATTACK_BUTTON_3_X, UI_ATTACK_BUTTON_Y }, 2, window_width_px, window_height_px);
    Entity attackButton4 = createUIAttackButton(renderer, { UI_ATTACK_BUTTON_4_X, UI_ATTACK_BUTTON_Y }, 3, window_width_px, window_height_px);
    
    // Need this to update attack button texts later
    size_t numEntities = registry.texts.entities.size();
    for (size_t i = numEntities - 16; i < numEntities; ++i)
    {
        attackButtonTextEntities.push_back(registry.texts.entities[i]);
    }
	registry.enemies.entities.clear(); 
	createUILine(renderer, 30.f);
	createUIPlayerHealth(renderer);
	createUIPlayerMana(renderer);
	createUIFPS(renderer);
	createUIPlayerHealthIcon(renderer);
	createUIPlayerManaIcon(renderer);
	// createFire(renderer);

    // NOTE: commenting these out for now, might re-introduce them later
    // createUIHeroHealthBar(renderer, {UI_ATTACK_BUTTON_1_X - UI_ATTACK_BUTTON_WIDTH/2 - UI_HP_BAR_WIDTH/2 - 10, UI_ATTACK_BUTTON_Y_UP});
    // createUIEnemyHealthBar(renderer, {UI_ATTACK_BUTTON_3_X + UI_ATTACK_BUTTON_WIDTH/2 + UI_HP_BAR_WIDTH/2 + 10, UI_ATTACK_BUTTON_Y_UP});

    // ROY: create HP bars
    // note: these have to be created after the hero and enemies are created
    // createUIHeroHealthBar(renderer, {UI_ATTACK_BUTTON_1_X - UI_ATTACK_BUTTON_WIDTH/2 - UI_HP_BAR_WIDTH/2 - 10, UI_ATTACK_BUTTON_Y_UP});
    // createUIEnemyHealthBar(renderer, {UI_ATTACK_BUTTON_3_X + UI_ATTACK_BUTTON_WIDTH/2 + UI_HP_BAR_WIDTH/2 + 10, UI_ATTACK_BUTTON_Y_UP});

    input_disabled = false;
    hero_dead = false;
    attack_to_use = 0;
    enemy_attack_to_use = 0;
    fade_timer = 0.f;
    user_selected_map = false;
    user_selected_combat_location = false;
    xchange = false;
    ychange = false;
    ScreenState& screen = registry.screenStates.components[0];
    screen.darken_screen_factor = 0;

    enemies_remaining_turns = registry.enemies.entities.size();

    AttackComponent& heroAttacks = registry.attacks.get(hero);
    heroAttacks.selectAttack(-1);
    // registry.list_all_components();
}


// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

void WorldSystem::updateAttackButtonText()
{
    // Get the hero entity
    Entity hero = registry.players.entities[0];

    // Ensure the hero has attacks
    if (!registry.attacks.has(hero)) {
        printf("Hero has no attacks available to update UI.\n");
        return;
    }

    // Retrieve the hero's attacks
    auto& heroAttacks = registry.attacks.get(hero).availableAttacks;

	printf("Size of attackButtonTextEntities: %zu\n", attackButtonTextEntities.size());

    // Iterate through each set of text entities (name, damage, mana cost)
    for (size_t i = 0; i < attackButtonTextEntities.size(); i += 4)
    {
        size_t attackIndex = i / 4; // Determine the corresponding attack index
        if (attackIndex >= heroAttacks.size()) {
            printf("Not enough attacks to update all buttons.\n");
            break;
        }

        // Get the current attack details
        const Attack& currentAttack = heroAttacks[attackIndex];

        // Update attack name
        auto& attackNameText = registry.texts.get(attackButtonTextEntities[i]);
        attackNameText.text = currentAttack.name;

        // Update damage text
        auto& damageText = registry.texts.get(attackButtonTextEntities[i + 1]);
        damageText.text = std::to_string(currentAttack.damage) + " dmg";

        // Update mana cost text
        auto& manaText = registry.texts.get(attackButtonTextEntities[i + 2]);
        manaText.text = std::to_string(currentAttack.manaCost) + " mana";

		// Update mana cost text
        auto& healText = registry.texts.get(attackButtonTextEntities[i + 3]);
        healText.text = std::to_string(currentAttack.heal) + " heal";

    }
}

int calculateDamage(Attack attack) {
	int damage = attack.damage;
	printf("Damage before calculation: %d\n", damage);
	for (const Modifier modifiers : attack.modifiers) {
		if (modifiers.modifierType == ModifierType::ADDDAMAGE) {
			damage += modifiers.effectModifier;
		} else if (modifiers.modifierType == ModifierType::MULTIPLYDAMAGE) {
			damage *= modifiers.effectModifier;
		}
	}
	printf("Damage after calculation: %d\n", damage);
	return damage;
}

void WorldSystem::handle_collisions() {
    if (registry.collisions.components.size() > 0) {
		Entity entity = registry.collisions.entities[0];
		Entity entity_other = registry.collisions.components[0].other;
		if (registry.mapEnemies.has(entity) || registry.mapEnemies.has(entity_other)) {
			Motion& heroMotion = registry.motions.get(hero);
			heroMotion.mapPosition = heroMotion.position;
			MapEnemy mapEnemy;
			if (registry.mapEnemies.has(entity_other)) {
				mapEnemy = registry.mapEnemies.get(entity_other);
				heroMotion.mapEnemy = entity_other;
			}
			else {
				mapEnemy = registry.mapEnemies.get(entity);
				heroMotion.mapEnemy = entity;
			}
			loadCombatStage(mapEnemy.combatStage);
			user_selected_combat_location = true;
		}

		// Player Wall Collision (Player bounces off walls)
		if (registry.walls.has(entity) || registry.walls.has(entity_other)) {
			Motion& motion = registry.motions.get(hero);
			Motion motion2;


			if (registry.walls.has(entity)) {
				motion2 = registry.motions.get(entity);
			}
			else if(registry.walls.has(entity_other)) {
				motion2 = registry.motions.get(entity_other);
			}

			if (xchange) {
				if (motion.position.x >= motion2.position.x) {
				motion.position.x += 20;
				} else if (motion.position.x < motion2.position.x) {
					motion.position.x -= 20;
				}
			
			}

			if (ychange) {
				if (motion.position.y <= motion2.position.y) {
					motion.position.y -= 20;
				} else if (motion.position.y > motion2.position.y) {
						motion.position.y += 20;
					}
				
			}
		}

		// Projectile collisions, if projectile collides with target then delete projectile
		if (registry.projectiles.has(entity) || registry.projectiles.has(entity_other)) {
			Entity projectile;
			Entity other;

			if (registry.projectiles.has(entity)) {
				projectile = entity;
				other = entity_other;
			} else if (registry.projectiles.has(entity_other)) {
				projectile = entity_other;
				other = entity;
			}

			Entity target = registry.projectiles.get(projectile).target;
			if (other == target) {
				if (target == hero) {
					if (registry.healths.has(hero)) {
						int damage = registry.projectiles.get(projectile).attack.damage;
            Health& player_health = registry.healths.get(hero);

            if (!registry.parrys.has(hero)) {
                player_health.hp -= damage;
                player_health.took_damage = true;
                Mix_PlayChannel(-1, hero_hit_sound, 0);
                registry.hits.get(hero).hit = true;
                registry.parrys.remove(hero);
                registry.parryCooldowns.remove(hero);
                parry_timer = 0.0;

							if (player_health.hp < 0) {
								player_health.hp = 0;
							}
        
							printf("Player health: %d\n", player_health.hp);

        			if (player_health.hp <= 0) {
          			printf("Player defeated!\n");
          			hero_dead = true;
        			}
        		}
          }
		    } else {
					// Apply damage to enemy
					if (registry.healths.has(target)) {
						Health& enemy_health = registry.healths.get(target);
						Attack selected_attack = registry.projectiles.get(projectile).attack;
						enemy_health.hp -= calculateDamage(selectedAttack);
						enemy_health.took_damage = true;
						Health& player_health = registry.healths.get(hero);
						if (player_health.hp + selectedAttack.heal > player_health.max_hp) {
							player_health.hp = player_health.max_hp;
						} else {
							player_health.hp += selectedAttack.heal;
						}
						Mix_PlayChannel(-1, enemy_hit_sound, 0);
						registry.hits.get(target).hit = true;
						if (enemy_health.hp < 0) {
							enemy_health.hp = 0;
						}
						printf("Enemy health: %d\n", enemy_health.hp);
						if (enemy_health.hp <= 0) {
							printf("Enemy defeated.\n");
							dead_enemy = target;
							Mix_PlayChannel(-1, enemy_death_sound, 0);
						}
					registry.targets.remove(target);
					}
        	registry.colors.get(target) = vec3(1.f, 1.f, 1.f); // Reset to white

					if (registry.projectiles.get(projectile).isMelee) {
						registry.players.get(hero).anim_type = AnimationType::IDLE;
					}
				}
				registry.remove_all_components_of(projectile);
				if (target != hero) {
					playerAttackInProgress = false;
    				nextTurn();
				}
			}
			}
    }

    // Remove all collisions from this simulation step
    registry.collisions.clear();
}

void WorldSystem::saveGame(const PlayerSaveData& data, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        printf("Failed to open file for saving: %s\n", filename.c_str());
        return;
    }

    // Save player stats
    file << data.currentHp << "\n";
    file << data.maxHp << "\n";
    file << data.currentMana << "\n";
    file << data.maxMana << "\n";
	file << mapCounter << "\n";

    // Save player attacks
    file << data.attacks.size() << "\n";
    for (const auto& attack : data.attacks) {
        file << attack.name << " " << attack.damage << " " << attack.manaCost << " "
             << attack.heal << " " << attack.affectedEnemy << " " << attack.usedHits << "\n";
    }

    file.close();
    printf("Game saved to file: %s\n", filename.c_str());
}

void WorldSystem::loadGame() {
    const std::string saveFilePath = "../data/savefile.txt";
    std::ifstream file(saveFilePath);

    if (!file.is_open()) {
        printf("Failed to open file for loading: %s\n", saveFilePath.c_str());
        return;
    }

    PlayerSaveData playerData;

    file >> playerData.currentHp;
    file >> playerData.maxHp;
    file >> playerData.currentMana;
    file >> playerData.maxMana;
	file >> mapCounter;

    size_t attackCount;
    file >> attackCount;
    playerData.attacks.clear();
    for (size_t i = 0; i < attackCount; ++i) {
        Attack attack;
        file >> attack.name;
        file >> attack.damage;
        file >> attack.manaCost;
        file >> attack.heal;
        file >> attack.affectedEnemy;
        file >> attack.usedHits;
        playerData.attacks.push_back(attack);
    }

    file.close();
    printf("Game loaded from file: %s\n", saveFilePath.c_str());

    if (registry.healths.has(hero)) {
        Health& heroHealth = registry.healths.get(hero);
        heroHealth.hp = playerData.currentHp;
        heroHealth.max_hp = playerData.maxHp;
    }

    if (registry.manas.has(hero)) {
        Mana& heroMana = registry.manas.get(hero);
        heroMana.currentMana = playerData.currentMana;
        heroMana.maxMana = playerData.maxMana;
    }

    if (registry.attacks.has(hero)) {
        registry.attacks.get(hero).availableAttacks = playerData.attacks;
    }

	updateAttackButtonText();

    current_game_state = GameState::MAP;
    renderer->game_state = GameState::MAP;
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {

	if (current_game_state == GameState::MAP) {
		float speed = 500;
		Motion& motion = registry.motions.get(hero);
		float current_x = motion.position.x;
		float current_y = motion.position.y;

		float half_width = motion.scale.x / 2;
		float half_height = motion.scale.y / 2;

		if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_DOWN) {
			motion.velocity.y = speed;
			ychange = true;
		}
		if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_UP) {
			motion.velocity.y = -speed;
			ychange = true;
		}
		if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_LEFT) {
			motion.velocity.x = -speed;
			xchange = true;
		}
		if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_RIGHT) {
			motion.velocity.x = speed;
			xchange = true;
		}

		if (action == GLFW_RELEASE) {
			motion.velocity.x = 0;
			motion.velocity.y = 0;
			xchange = false;
			ychange = false;
		}

		if (action == GLFW_RELEASE) {
			motion.velocity.x = 0;
			motion.velocity.y = 0;
		}
	}

	if (action == GLFW_RELEASE) {
        
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }

		// if (key == GLFW_KEY_Q) {
		// 	if (!registry.parrys.has(hero) && !!registry.parryCooldowns.has(hero)) {
		// 		registry.parrys.emplace(hero);
		// 		std::cout << "Hero has begun parrying" << std::endl;
		// 	}
		// }

		// Loading Game
		// if (key == GLFW_KEY_APOSTROPHE && current_game_state != GameState::PLAYING) {
		// 	// Need to remove all enemies on screen
		// 	for (Entity e : registry.mapEnemies.entities) {
		// 		if (registry.mapEnemies.has(e)) {
		// 			registry.remove_all_components_of(e);
		// 		}
		// 	}
        //     loadGame();
        //     printf("Game loaded from savefile.\n");
        // }

		// Saving game
		// if (key == GLFW_KEY_SEMICOLON) {
		// 	PlayerSaveData playerData;

		// 	if (registry.healths.has(hero)) {
		// 		const Health& heroHealth = registry.healths.get(hero);
		// 		playerData.currentHp = heroHealth.hp;
		// 		playerData.maxHp = heroHealth.max_hp;
		// 	}

		// 	if (registry.manas.has(hero)) {
		// 		const Mana& heroMana = registry.manas.get(hero);
		// 		playerData.currentMana = heroMana.currentMana;
		// 		playerData.maxMana = heroMana.maxMana;
		// 	}

		// 	if (registry.attacks.has(hero)) {
		// 		const auto& heroAttacks = registry.attacks.get(hero).availableAttacks;
		// 		playerData.attacks = heroAttacks;
		// 	}

		// 	const std::string saveFilePath = "../data/savefile.txt";
		// 	saveGame(playerData, saveFilePath);
		// 	printf("Game saved to: %s\n", saveFilePath.c_str());
		// }

		// if (key == GLFW_KEY_T) {
		// 	user_selected_map = false;
		// 	renderer->setGameState(GameState::TUTORIAL);
		// 	current_game_state = GameState::TUTORIAL;
		// }

		// if (key == GLFW_KEY_P) {
		// 	user_selected_map = true;
		// 	renderer->setGameState(GameState::MAP);
		// 	current_game_state = GameState::MAP;
		// }

		if (key == GLFW_KEY_M) {
			user_selected_map = false;
			user_selected_tutorial = false;
			renderer->setGameState(GameState::MENU);
			current_game_state = GameState::MENU;
		}

        if (key == GLFW_KEY_R && current_turn != TurnState::ENEMY_TURN) {
            printf("Restarting game...\n");
            current_turn = TurnState::PLAYER_TURN;
			fade_timer = 3000;
            reset_game();
            return;
        }

		if (key == GLFW_KEY_S) {
			for (Entity entity : registry.mapEnemies.entities) {
				registry.remove_all_components_of(entity);
			}
			return;
		}

		// if (key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3 || key == GLFW_KEY_4){
		// 	Motion& hero_motion = registry.motions.get(hero);
		// 	if (!hero_dead) {
		// 		if (current_turn == TurnState::PLAYER_TURN) {
		// 		AttackComponent& heroAttacks = registry.attacks.get(hero);
		// 		if (key == GLFW_KEY_1) {
		// 			printf("Player uses attack 1\n");
		// 			heroAttacks.selectAttack(0);
		// 		}
		// 		else if (key == GLFW_KEY_2) {
		// 			printf("Player uses attack 2\n");
		// 			heroAttacks.selectAttack(1);
		// 		}
		// 		else if (key == GLFW_KEY_3) {
		// 			printf("Player uses attack 3\n");
		// 			heroAttacks.selectAttack(2);
		// 		}
		// 		else if (key == GLFW_KEY_4) {
		// 			printf("Player uses attack 4\n");
		// 			heroAttacks.selectAttack(3);
		// 		}
		// 	}
		// 	AttackComponent& heroAttacks = registry.attacks.get(hero);
		// 	Attack& selectedAttack = heroAttacks.getCurrentAttack();

		// 	if (registry.manas.has(hero)) {
		// 		Mana& heroMana = registry.manas.get(hero);
		// 		if (heroMana.currentMana >= selectedAttack.manaCost) {

		// 			// Hero has enough mana, deduct mana and proceed with the attack
		// 			int i = 1;
		// 			bool targetAvailable = false;
		// 			for (Entity enemy : registry.enemies.entities) {
		// 				if (i == selectedAttack.affectedEnemy) {
		// 					targetAvailable = true;
		// 					if (!registry.targets.has(enemy)) {
		// 						registry.targets.emplace(enemy);
		// 					}
		// 				}
		// 				i++;
		// 			}
		// 			if (targetAvailable) {
		// 				heroMana.currentMana -= selectedAttack.manaCost;
		// 				// hero_motion.velocity = { 500.f, 0.f };
		// 				Entity first_enemy = registry.enemies.entities[0];
		// 				playerAttack(attack_to_use, first_enemy);
		// 				input_disabled = false;
		// 				input_disabled = true;
		// 				attack_to_use = 0;
		// 				printf("Hero used %s (Mana left: %d)\n", selectedAttack.name.c_str(), heroMana.currentMana);
		// 			}
		// 		} else {
		// 			printf("Not enough mana to use %s! (Mana needed: %d, Mana available: %d)\n",
		// 			selectedAttack.name.c_str(), selectedAttack.manaCost, heroMana.currentMana);
		// 		}
		// 	}
		// }
        
        // }
    }
}


void WorldSystem::on_mouse_move(vec2 mouse_position) {
	AttackComponent& heroAttacks = registry.attacks.get(hero);

    if (isChoosingTarget && heroAttacks.selectedAttackIndex != -1 && current_turn == TurnState::PLAYER_TURN) {
        for (Entity enemy : registry.enemies.entities) {
            Motion& enemyMotion = registry.motions.get(enemy);
            vec2 enemyPos = enemyMotion.position;
            vec2 enemySize = enemyMotion.scale;
			
            // Is mouse hovering enemy?
            if (mouse_position.x >= enemyPos.x - enemySize.x / 2 &&
                mouse_position.x <= enemyPos.x + enemySize.x / 2 &&
                mouse_position.y >= enemyPos.y - enemySize.y / 2 &&
                mouse_position.y <= enemyPos.y + enemySize.y / 2) {
				
				// Highlight the enemy in green
                registry.colors.get(enemy) = vec3(0.f, 1.f, 0.f);
            } else {
                registry.colors.get(enemy) = vec3(1.f, 1.f, 1.f); 
            }
        }
    }

	if (current_turn == TurnState::PLAYER_TURN) {
		for (Entity button : registry.buttons.entities) {
			auto& ui = registry.uis.get(button);

			if (mouse_position.x >= ui.position.x - ui.scale.x / 2 &&
				mouse_position.x <= ui.position.x + ui.scale.x / 2 &&
				mouse_position.y >= ui.position.y - ui.scale.y / 2 &&
				mouse_position.y <= ui.position.y + ui.scale.y / 2) {

				if (registry.colors.has(button)) {
					registry.colors.get(button) = vec3(0.f, 1.f, 0.f); // turn green
				} else {
					registry.colors.emplace(button, vec3(0.f, 1.f, 0.f));  
				}
			} else {
				if (registry.colors.has(button)) {
					registry.colors.get(button) = vec3(1.f, 1.f, 1.f);
				}
			}
		}
	}

	if (current_game_state == GameState::LOOT) {
		double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        ypos = window_height_px - ypos;

        for (size_t i = 0; i < renderer->lootBoundingBoxes.size(); ++i) {
            const vec4& box = renderer->lootBoundingBoxes[i]; 

            if (xpos >= box.x && xpos <= box.x + box.z && ypos >= box.y && ypos <= box.y + box.w) {
                renderer->lootHover = i;
				break;
            } else{
				renderer->lootHover = 9;
			}
        }
	}

}

void WorldSystem::resetPlayerAttacks() {
    // Get the player entity
    Entity player = registry.players.entities[0];

    // Reset the player's attacks to the original starting 4 attacks
    registry.attacks.get(player) = AttackComponent{
        {
            Attack{"Basic", 15, 0, 1, 1, 15, false}, 
            Attack{"Tackle", 25, 20, 2, 1, 0, false}, 
            Attack{"Heavy", 30, 35, 1, 1, 0, false}, 
            Attack{"BodySlam", 50, 100, 2, 1, 0, false}
        }
    };
}

void WorldSystem::finishLoot(){
	removedAttack = false;
	renderer->newAttackSelected = false;
	user_selected_loot = false;
	Mix_PlayChannel(-1, loot_sound, 0);
	current_game_state = GameState::MAP;
	renderer->game_state = GameState::MAP;
	selectedLootIndex = -1; // Reset the index
	renderer->selectedLoot = "";
	Motion& hero_motion = registry.motions.get(hero);
	registry.renderRequestsMap.remove(hero_motion.mapEnemy);
	registry.mapEnemies.remove(hero_motion.mapEnemy);
	hero_motion.position = hero_motion.mapPosition;
}

// Callback for handling mouse click events
void WorldSystem::on_mouse_click(GLFWwindow* window, int button, int action, int mods) {

	if (!removedAttack && user_chose_attack) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		const LootOption& selectedOption = currentLootOptions[selectedLootIndex];

		if ((UI_ATTACK_BUTTON_1_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_1_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
			(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
			Attack newAttack = selectedOption.attackEffect();
			printf("New attack applied: %s (Damage: %d, Mana Cost: %d)\n",
				newAttack.name.c_str(), newAttack.damage, newAttack.manaCost);
			addNewAttackToHero(newAttack, 0);
			removedAttack = true;
			user_chose_attack = false;
			finishLoot();
		}

		if ((UI_ATTACK_BUTTON_2_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_2_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
			(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
			Attack newAttack = selectedOption.attackEffect();
			printf("New attack applied: %s (Damage: %d, Mana Cost: %d)\n",
				newAttack.name.c_str(), newAttack.damage, newAttack.manaCost);
			addNewAttackToHero(newAttack, 1);
			removedAttack = true;
			user_chose_attack = false;
			finishLoot();
		}

		if ((UI_ATTACK_BUTTON_3_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_3_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
			(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
			Attack newAttack = selectedOption.attackEffect();
			printf("New attack applied: %s (Damage: %d, Mana Cost: %d)\n",
				newAttack.name.c_str(), newAttack.damage, newAttack.manaCost);
			addNewAttackToHero(newAttack, 2);
			removedAttack = true;
			user_chose_attack = false;
			finishLoot();
		}

		if ((UI_ATTACK_BUTTON_4_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_4_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
			(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
			Attack newAttack = selectedOption.attackEffect();
			printf("New attack applied: %s (Damage: %d, Mana Cost: %d)\n",
				newAttack.name.c_str(), newAttack.damage, newAttack.manaCost);
			addNewAttackToHero(newAttack, 3);
			removedAttack = true;
			user_chose_attack = false;
			finishLoot();
		}
	}

	if (user_selected_loot) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		if ((UI_LOOT_BUTTON_X - UI_MAP_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_LOOT_BUTTON_X + UI_MAP_BUTTON_WIDTH / 2) &&
			(UI_LOOT_BUTTON_Y - UI_MAP_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_LOOT_BUTTON_Y + UI_MAP_BUTTON_HEIGHT / 2)) {

			if (selectedLootIndex != -1 && selectedLootIndex < currentLootOptions.size()) {
				const LootOption& selectedOption = currentLootOptions[selectedLootIndex];

				// Apply the selected loot effect
				if (selectedOption.attackEffect) {
					renderer->newAttackSelected = true;
					user_chose_attack = true;

					if (!removedAttack) {
						return;
					} 
				} else if (selectedOption.voidEffect) {
					selectedOption.voidEffect();
					updateAttackButtonText();
					finishLoot();
				}

				// Reset state and clear the selection
				// finishLoot();
			} else {
				printf("Error: Invalid loot option index.\n");
			}
		}
	}

	if (current_game_state == GameState::LOOT && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        ypos = window_height_px - ypos;

        printf("Mouse clicked at: (%.1f, %.1f)\n", xpos, ypos);

        for (size_t i = 0; i < renderer->lootBoundingBoxes.size(); ++i) {
            const vec4& box = renderer->lootBoundingBoxes[i]; 

            if (xpos >= box.x && xpos <= box.x + box.z && ypos >= box.y && ypos <= box.y + box.w) {
                selectedLootIndex = i;
				renderer->newAttackSelected = false;
                renderer->selectedLoot = currentLootOptions[i].name;
                printf("Option %zu selected: %s\n", i, renderer->selectedLoot.c_str());
                user_selected_loot = true;
				user_chose_attack = false;
                break;
            }
        }

        if (selectedLootIndex == -1) {
            printf("No loot option selected.\n");
        }
    }

	if (current_turn == TurnState::PLAYER_TURN) {
		Motion& hero_motion = registry.motions.get(hero);
		Mana& heroMana = registry.manas.get(hero);
		AttackComponent& heroAttacks = registry.attacks.get(hero);

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			if (!hero_dead && current_game_state == GameState::PLAYING && !input_disabled && current_turn == TurnState::PLAYER_TURN && !playerAttackInProgress) {

				if ((UI_ATTACK_BUTTON_1_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_1_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
					(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
					heroAttacks.selectAttack(0);
				}

				if ((UI_ATTACK_BUTTON_2_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_2_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
					(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
					heroAttacks.selectAttack(1);
				}

				if ((UI_ATTACK_BUTTON_3_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_3_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
					(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
					heroAttacks.selectAttack(2);
				}

				if ((UI_ATTACK_BUTTON_4_X - UI_ATTACK_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_ATTACK_BUTTON_4_X + UI_ATTACK_BUTTON_WIDTH / 2) &&
					(UI_ATTACK_BUTTON_Y - UI_ATTACK_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_ATTACK_BUTTON_Y + UI_ATTACK_BUTTON_HEIGHT / 2)) {
					heroAttacks.selectAttack(3);
				}

				if (registry.manas.has(hero) && heroAttacks.selectedAttackIndex != -1) {
					selectedAttack = heroAttacks.getCurrentAttack();

					if (heroMana.currentMana >= selectedAttack.manaCost) {
						printf("Attack selected: %s\n", selectedAttack.name.c_str());
						isChoosingTarget = true;
					} else {
						printf("Insufficient mana for selected attack: %s (Mana Cost: %d, Available: %d)\n",
							selectedAttack.name.c_str(), selectedAttack.manaCost, heroMana.currentMana);

						isChoosingTarget = false;
						heroAttacks.selectAttack(-1);
					}
				}
			}
		}

		if (isChoosingTarget && heroAttacks.selectedAttackIndex != -1) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			if (heroMana.currentMana < selectedAttack.manaCost) {
				printf("Cannot attack: Insufficient mana for %s! (Mana needed: %d, Mana available: %d)\n",
					selectedAttack.name.c_str(), selectedAttack.manaCost, heroMana.currentMana);

				isChoosingTarget = false;
				heroAttacks.selectAttack(-1);
				return;
			}

			for (Entity enemy : registry.enemies.entities) {
				Motion& enemyMotion = registry.motions.get(enemy);
				vec2 enemyPos = enemyMotion.position;
				vec2 enemySize = enemyMotion.scale;

				if (xpos >= enemyPos.x - enemySize.x / 2 && xpos <= enemyPos.x + enemySize.x / 2 &&
					ypos >= enemyPos.y - enemySize.y / 2 && ypos <= enemyPos.y + enemySize.y / 2) {

					heroMana.currentMana -= selectedAttack.manaCost;
					playerAttack(attack_to_use, enemy);
					printf("Hero used %s (Mana left: %d)\n", selectedAttack.name.c_str(), heroMana.currentMana);

					isChoosingTarget = false;
					heroAttacks.selectAttack(-1);
					return;
				}
			}
		}
	}

	double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

	if (current_game_state == GameState::MENU) {
				
		// Check Menu Button 1, restart button
		if ((UI_MENU_BUTTON_1_X - UI_MAP_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_MENU_BUTTON_1_X + UI_MAP_BUTTON_WIDTH / 2) &&
			(UI_MENU_BUTTON_1_Y - UI_MAP_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_MENU_BUTTON_1_Y + UI_MAP_BUTTON_HEIGHT / 2)) {
			printf("Restarting game...\n");
            current_turn = TurnState::PLAYER_TURN;
			fade_timer = 3000;
            reset_game();
            return;
		}
				
		// Check Menu Button 2, exit game
		if ((UI_MENU_BUTTON_2_X - UI_MAP_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_MENU_BUTTON_2_X + UI_MAP_BUTTON_WIDTH / 2) &&
			(UI_MENU_BUTTON_2_Y - UI_MAP_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_MENU_BUTTON_2_Y + UI_MAP_BUTTON_HEIGHT / 2)) {
			glfwSetWindowShouldClose(window, true);
		}

		// Check Menu Button 3, Play game
		if ((UI_MENU_BUTTON_3_X - UI_MAP_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_MENU_BUTTON_3_X + UI_MAP_BUTTON_WIDTH / 2) &&
			(UI_MENU_BUTTON_3_Y - UI_MAP_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_MENU_BUTTON_3_Y + UI_MAP_BUTTON_HEIGHT / 2)) {
			renderer->setGameState(GameState::MAP);
			current_game_state = GameState::MAP;
		}

		// Check Menu Button 4, Nothing right now
		if ((UI_MENU_BUTTON_4_X - UI_MAP_BUTTON_WIDTH / 2 <= xpos) && (xpos <= UI_MENU_BUTTON_4_X + UI_MAP_BUTTON_WIDTH / 2) &&
			(UI_MENU_BUTTON_4_Y - UI_MAP_BUTTON_HEIGHT / 2 <= ypos) && (ypos <= UI_MENU_BUTTON_4_Y + UI_MAP_BUTTON_HEIGHT / 2)) {
			return;
		}
			
	}
}

// This function applies damage to all entities that have a Health component and have been attacked.
void WorldSystem::applyDamageSystem() {
    // Loop through all entities that have Health
    for (Entity entity : registry.healths.entities) {
        if (registry.damages.has(entity)) {
            // Health and Damage components
			if (registry.targets.has(entity)) {
				Health& health = registry.healths.get(entity);
				Damage& damage = registry.damages.get(entity);

				health.hp -= damage.amount;

				if (health.hp <= 0) {
					health.hp = 0;
					if (entity == hero) {
						printf("Hero defeated!\n");
						hero_dead = true;
						fade_timer = 0.f;  // Reset fade timer
					}
					else {
						printf("Entity %u has died!\n", (unsigned int)entity);
						registry.enemies.remove(entity);
					}
				}

				// Once damage is applied, remove the component
				registry.damages.remove(entity);
				registry.targets.remove(entity);
			}
        }
    }
}

// KEVIN: Swap attacks/abilities
void WorldSystem::addNewAttackToHero(Attack newAttack, int index) {
    if (registry.attacks.has(hero)) {
        AttackComponent& heroAttacks = registry.attacks.get(hero);
        
        if (heroAttacks.availableAttacks.size() >= 4) {
            // Later we can prompt player here, for now swap 1st attack
            printf("Hero already has 4 attacks. Replacing the first one with: %s\n", newAttack.name.c_str());
            heroAttacks.availableAttacks[index] = newAttack;
			updateAttackButtonText();
        } else {
            // Should never go here
            heroAttacks.availableAttacks.push_back(newAttack);
            printf("Added new attack: %s\n", newAttack.name.c_str());
        }
    }
}

// KEVIN: Combat system
void WorldSystem::nextTurn() {

	if (playerAttackInProgress) {
        return; 
    }

    if (current_turn == TurnState::PLAYER_TURN) {
        // Player turn ends, switch to enemy's turn
        current_turn = TurnState::ENEMY_TURN;
    } else {
        // Enemy turn ends, switch back to player's turn
        current_turn = TurnState::PLAYER_TURN;
    }
}

// KEVIN: combat system
void WorldSystem::playerAttack(int attack_id, Entity enemy) {

	bool hasUsableAttack = false;
	AttackComponent& heroAttacks = registry.attacks.get(hero);
	Mana& heroMana = registry.manas.get(hero);

	for (const Attack& attack : heroAttacks.availableAttacks) {
		if (heroMana.currentMana >= attack.manaCost) {
			hasUsableAttack = true;
			break;
		}
	}

	if (!hasUsableAttack) {
		renderer->game_state = GameState::GAMEOVER;	
		renderer->noMana = true;
		return;
	}

	Motion& motion = registry.motions.get(hero);
    Attack& selectedAttack = heroAttacks.getCurrentAttack();

	playerAttackInProgress = true;

	printf("Player uses attack: %s (Damage: %d, Mana Cost: %d)\n", selectedAttack.name.c_str(), selectedAttack.damage, selectedAttack.manaCost);
    
		// Start hero animation
		if (selectedAttack.name == "Basic") {
			registry.players.get(hero).anim_type = AnimationType::BASIC;
			createBasicProjectile(renderer, registry.motions.get(hero).position, selectedAttack, enemy);

		} else if (selectedAttack.name == "Tackle") {
			registry.players.get(hero).anim_type = AnimationType::HERO_MELEE;
			createTackle(renderer, registry.motions.get(hero).position, selectedAttack, enemy);
			Mix_PlayChannel(-1, woosh_sound, 0);

		} else if (selectedAttack.name == "Heavy") {
			registry.players.get(hero).anim_type = AnimationType::HERO_MELEE;
			createHeavy(renderer, registry.motions.get(hero).position, selectedAttack, enemy);
			Mix_PlayChannel(-1, woosh_sound, 0);

		} else if (selectedAttack.name == "BodySlam") {
			registry.players.get(hero).anim_type = AnimationType::HERO_MELEE;
			createBodyslam(renderer, registry.motions.get(hero).position, selectedAttack, enemy);
			Mix_PlayChannel(-1, woosh_sound, 0);
		} else if (selectedAttack.name == "Fireball") {
			registry.players.get(hero).anim_type = AnimationType::FIREBALL;
			createFireball(renderer, registry.motions.get(hero).position, selectedAttack, enemy);
			Mix_PlayChannel(-1, fireball_sound, 0);
		} else if (selectedAttack.name == "Lightning") {
			registry.players.get(hero).anim_type = AnimationType::LIGHTNING;
			Mix_PlayChannel(-1, lightning_sound, 0);
			createLightning(renderer, selectedAttack, enemy);
		} else if (selectedAttack.name == "IceShard") {
			registry.players.get(hero).anim_type = AnimationType::ICESHARD;
			Mix_PlayChannel(-1, arrow_sound, 0);
			createIceShard(renderer, registry.motions.get(hero).position, selectedAttack, enemy);
		} else if (selectedAttack.name == "Zap") {
			registry.players.get(hero).anim_type = AnimationType::ZAP;
			Mix_PlayChannel(-1, zap_sound, 0);
		} else if (selectedAttack.name == "Quake") {
			registry.players.get(hero).anim_type = AnimationType::QUAKE;
			Mix_PlayChannel(-1, quake_sound, 0);
		} else if (selectedAttack.name == "Lifetap") {
			registry.players.get(hero).anim_type = AnimationType::LIFETAP;
			createLifetap(renderer, registry.motions.get(hero).position, selectedAttack, enemy);
		}

		// only apply damage immediately for non "projectile" attacks:
		// NOTE: the true case below is for debugging purposes rn because projectiles don't collide with enemies
		if (selectedAttack.name == "Zap" || selectedAttack.name == "Quake") {
			// Apply damage to enemy
			if (registry.healths.has(enemy)) {
				Health& enemy_health = registry.healths.get(enemy);
				enemy_health.hp -= calculateDamage(selectedAttack);
				enemy_health.took_damage = true;
				Health& player_health = registry.healths.get(hero);
				if (player_health.hp + selectedAttack.heal > player_health.max_hp) {
					player_health.hp = player_health.max_hp;
				} else {
					player_health.hp += selectedAttack.heal;
				}
				Mix_PlayChannel(-1, enemy_hit_sound, 0);
				registry.hits.get(enemy).hit = true;
				if (enemy_health.hp < 0) {
					enemy_health.hp = 0;
				}
				printf("Enemy health: %d\n", enemy_health.hp);
				if (enemy_health.hp <= 0) {
					printf("Enemy defeated.\n");
					dead_enemy = enemy;
					Mix_PlayChannel(-1, enemy_death_sound, 0);
				}
			registry.targets.remove(enemy);
			playerAttackInProgress = false;
    		nextTurn();	
    	}
		}

	for (Entity e : registry.enemies.entities) {
        registry.colors.get(e) = vec3(1.f, 1.f, 1.f); // Reset to white
    }
    // nextTurn();
	printf("---------------------------------------------------\n");
}

bool WorldSystem::canAttackKillPlayer(Entity enemy, Entity player) {
    if (!registry.healths.has(player)) {
        return false;
    }
    const std::vector<Attack> attacks = getAvailableAttacksForEnemyType(registry.enemyTypes.get(enemy).type);
    const Health& playerHealth = registry.healths.get(player);
  
    for (const Attack& attack : attacks) {
        if (playerHealth.hp <= attack.damage) {
            return true; // An attack will kill player
        }
    }
    return false;
}

int WorldSystem::getBestAttackIndex(const std::vector<Attack>& attacks) {
    int bestIndex = 0;
    int maxDamage = 0;

    for (size_t i = 0; i < attacks.size(); ++i) {
        if (attacks[i].damage > maxDamage) {
            maxDamage = attacks[i].damage;
            bestIndex = static_cast<int>(i);
        }
    }
    return bestIndex;
}

Attack WorldSystem::getHealingAttack(const std::vector<Attack>& attacks) {
    for (const Attack& attack : attacks) {
        if (attack.heal > 0) {
            return attack;
        }
    }
    return Attack{};
}

void WorldSystem::applyHeal(Entity healer, Entity target, const Attack& healAttack) {
    if (registry.healths.has(target) && target != healer) {
        Health& targetHealth = registry.healths.get(target);
		if (registry.healths.has(target)) {
			Health& targetHealth = registry.healths.get(target);
			if (mapCounter <= 3) {
				targetHealth.hp += selectedAttack.heal * 0.5;
			}
			else if (mapCounter <= 5) {
				targetHealth.hp += selectedAttack.heal;
			}
			else if (mapCounter <= 7) {
				targetHealth.hp += selectedAttack.heal * 1.5;
			}
			else if (mapCounter <= 9) {
				targetHealth.hp += selectedAttack.heal * 2;
			}
		}

        // Prevent overhealing
        if (targetHealth.hp > targetHealth.max_hp) {
            targetHealth.hp = targetHealth.max_hp;
        }
		registry.enemies.get(healer).anim_type = AnimationType::HEAL;
		Mix_PlayChannel(-1, heal_sound, 0);
    }
}

Entity WorldSystem::findLowestHpAlly(Entity healer) {
    Entity lowestHpAlly = healer;
    int lowestHp = registry.healths.get(healer).hp;

    for (Entity ally : registry.enemies.entities) {
        if (ally != healer && registry.healths.has(ally)) {
            const Health& allyHealth = registry.healths.get(ally);
            if (allyHealth.hp < lowestHp && allyHealth.hp < allyHealth.max_hp) {
                lowestHpAlly = ally;
                lowestHp = allyHealth.hp;
            }
        }
    }
    return lowestHpAlly;
}

void WorldSystem::enemyAttack(int attack_id, Entity enemyEntity) {

	EnemyType& enemyType = registry.enemyTypes.get(enemyEntity);
	Motion& motion = registry.motions.get(enemyEntity);
	motion.position = motion.originalPosition;

    std::vector<Attack> availableAttacks = getAvailableAttacksForEnemyType(enemyType.type);
    Attack& selectedAttack = availableAttacks[attack_id];

     if (selectedAttack.heal > 0) {
        Entity targetToHeal = findLowestHpAlly(enemyEntity);
        if (registry.healths.has(targetToHeal) && registry.enemies.has(targetToHeal)) {
            applyHeal(enemyEntity, targetToHeal, selectedAttack);
        }
    } else if (selectedAttack.isRanged) {
		int damage = selectedAttack.damage;
			// Select corresponding ranged animation, and defer applying damage to collision handling
			// TODO: add check on enemy type once we no longer are using the default boss sprite for everything
			if (selectedAttack.name == "Fireball" || selectedAttack.name == "Big Fireball" || selectedAttack.name == "Very Big Fireball" ) {
				printf("Enemy uses attack %s: deals %d damage to player\n", selectedAttack.name.c_str(), damage);
				registry.enemies.get(enemyEntity).anim_type = AnimationType::FIREBALL;
				createEnemyFireball(renderer, registry.motions.get(enemyEntity).position, selectedAttack, hero);
				Mix_PlayChannel(-1, fireball_sound, 0);
			}
			else if (selectedAttack.name == "Star Shot") {
				printf("Enemy uses attack %s: deals %d damage to player\n", selectedAttack.name.c_str(), damage);
				createEnemyStar(renderer, registry.motions.get(enemyEntity).position, selectedAttack, hero);
			}
			else if (selectedAttack.name == "Ice Spear") {
				printf("Enemy uses attack %s: deals %d damage to player\n", selectedAttack.name.c_str(), damage);
				createEnemyIceSpear(renderer, registry.motions.get(enemyEntity).position, selectedAttack, hero);
			}
			else {
				// This is here temporarily to handle applying damage to not yet implemented ranged attacks
        printf("Enemy uses attack %s: deals %d damage to player\n", selectedAttack.name.c_str(), damage);

        // Apply damage to the player
        if (registry.healths.has(hero)) {
            Health& player_health = registry.healths.get(hero);

            if (!registry.parrys.has(hero)) {
                player_health.hp -= damage;
                player_health.took_damage = true;
                Mix_PlayChannel(-1, hero_hit_sound, 0);
                registry.hits.get(hero).hit = true;
                registry.parrys.remove(hero);
                registry.parryCooldowns.remove(hero);
                parry_timer = 0.0;

				if (player_health.hp < 0) {
					player_health.hp = 0;
				}

                printf("Player health: %d\n", player_health.hp);

                if (player_health.hp <= 0) {
                    printf("Player defeated!\n");
                    hero_dead = true;
                }
            }
        }
			}
		} else {
        // Regular attacks
        int damage = selectedAttack.damage;
        printf("Enemy uses attack %s: deals %d damage to player\n", selectedAttack.name.c_str(), damage);

        // Apply damage to the player
        if (registry.healths.has(hero)) {
            Health& player_health = registry.healths.get(hero);

            if (!registry.parrys.has(hero)) {

                player_health.hp -= damage;
				if (mapCounter <= 3) {
					player_health.hp -= damage * 0.5;
				}
				else if (mapCounter <= 5) {
					player_health.hp -= damage * 0.75;
				}
				else if (mapCounter <= 7) {
					player_health.hp -= damage * 1;
				}
				else if (mapCounter <= 9) {
					player_health.hp -= damage * 1.25;
				}
                player_health.took_damage = true;
                Mix_PlayChannel(-1, hero_hit_sound, 0);
                registry.hits.get(hero).hit = true;
                registry.parrys.remove(hero);
                registry.parryCooldowns.remove(hero);
                parry_timer = 0.0;

                printf("Player health: %d\n", player_health.hp);

                if (player_health.hp <= 0) {
                    printf("Player defeated!\n");
                    hero_dead = true;
                }
            }
        }
    }

  // Animation for Tank
	if (enemyType.type == "Tank" && selectedAttack.name == "Punch") {
		registry.enemies.get(enemyEntity).anim_type = AnimationType::PUNCH;
		Mix_PlayChannel(-1, punch_sound, 0);
	}
	else if (enemyType.type == "Tank" && selectedAttack.name == "Heavy Punch") {
		registry.enemies.get(enemyEntity).anim_type = AnimationType::HEAVYPUNCH;
		Mix_PlayChannel(-1, punch_sound, 0);
	}
	else if (enemyType.type == "Healer" && selectedAttack.name == "Heal") {
		registry.enemies.get(enemyEntity).anim_type = AnimationType::HEAL;
		Mix_PlayChannel(-1, heal_sound, 0);
	}
	else if (enemyType.type == "Healer" && (selectedAttack.name == "Fireball" || selectedAttack.name == "Ice Spear")) {
		registry.enemies.get(enemyEntity).anim_type = AnimationType::SPELL;
	}
	else if (enemyType.type == "Ranged") {
		registry.enemies.get(enemyEntity).anim_type = AnimationType::RANGED;
		Mix_PlayChannel(-1, arrow_sound, 0);
	} else if (enemyType.type == "Melee") {
		if (selectedAttack.name == "Punch") {
			registry.enemies.get(enemyEntity).anim_type = AnimationType::MELEE_PUNCH;
			Mix_PlayChannel(-1, punch_sound, 0);
		} else if (selectedAttack.name == "Slash") {
			registry.enemies.get(enemyEntity).anim_type = AnimationType::SLASH;
			Mix_PlayChannel(-1, blade_sound, 0);
		} else if (selectedAttack.name == "Heavy Hit") {
			registry.enemies.get(enemyEntity).anim_type = AnimationType::HEAVY_HIT;
			Mix_PlayChannel(-1, punch_sound, 0);
		}
	}

    // Check for enemy health and handle victory if needed
    if (registry.healths.has(enemyEntity)) {
        Health& enemy_health = registry.healths.get(enemyEntity);
        if (enemy_health.hp <= 0) {
            printf("Victory! Enemy defeated.\n");
            current_game_state = GameState::VICTORY;
            return;
        }
    }

	if (current_turn == TurnState::PLAYER_TURN) {
		printf("Player turn\n");
	} else {
		printf("enemy turn!\n");
	}
	std::cout << enemies_remaining_turns << std::endl;
	printf("---------------------------------------------------\n");
}

// KEVIN: combat system
void WorldSystem::enemyTurn() {
    static size_t currentEnemyIndex = 0;   // Track which enemy is taking its turn
    static float enemyDelayTimer = 0.0f;  // Timer to delay between enemy actions

	if (playerAttackInProgress) {
		return;
	}
	
    if (current_turn != TurnState::ENEMY_TURN) {
        return; // Exit if it's not the enemy's turn
    }

    // Handle delay between enemy actions
    if (waiting) {
        return;
    }

    Entity playerEntity = hero;

    // Process the current enemy
    if (currentEnemyIndex < registry.enemies.entities.size()) {
        Entity enemy = registry.enemies.entities[currentEnemyIndex];

        // Skip dead enemies
        if (registry.healths.has(enemy) && registry.healths.get(enemy).hp <= 0) {
            currentEnemyIndex++;
            return;
        }

        // Execute decision tree for current enemy
        if (registry.enemyTypes.has(enemy)) {
            EnemyType& enemyType = registry.enemyTypes.get(enemy);

            // Select and execute the correct decision tree
            if (enemyType.type == "Healer") {
                HealerDecisionTree decisionTree;
                decisionTree.execute(*this, enemy, playerEntity);
            } else {
                DamageDealerDecisionTree decisionTree;
                decisionTree.execute(*this, enemy, playerEntity);
            }
        }

        // Move to the next enemy and trigger delay
        currentEnemyIndex++;
        waiting = true;
    }

    // If all enemies have taken their turns, switch back to the player's turn
    if (currentEnemyIndex >= registry.enemies.entities.size()) {
        currentEnemyIndex = 0;
        nextTurn();
    }
}

void WorldSystem::clearWalls() {
	for (Entity wall : registry.walls.entities) {
		if (registry.walls.has(wall)) {
			registry.remove_all_components_of(wall);
		}
	}
}