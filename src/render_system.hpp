#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"
#include "world_system.hpp"
#include "game_states.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>				// map of character textures

#include <iostream>
#include <assert.h>
#include <fstream>			// for ifstream
#include <sstream>			// for ostringstream

// font character structure
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON, mesh_path("salmon.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("/hero/idle/idle_0.png"),
			textures_path("/hero/idle/idle_1.png"),
			textures_path("/hero/idle/idle_2.png"),
			textures_path("/hero/idle/idle_3.png"),
			textures_path("empty.png"),
			textures_path("/hero/basic/basic_0.png"),
			textures_path("/hero/basic/basic_1.png"),
			textures_path("/hero/basic/basic_2.png"),
			textures_path("/hero/basic/basic_3.png"),
			textures_path("/hero/basic/basic_4.png"),
			textures_path("/hero/basic/basic_5.png"),
			textures_path("/hero/basic/basic_6.png"),
			textures_path("/hero/basic/basic_7.png"),
			textures_path("/hero/basic/basic_projectile.png"),
			textures_path("/hero/tackle/tackle_0.png"),
			textures_path("/hero/heavy/heavy_0.png"),
			textures_path("/hero/bodyslam/bodyslam_0.png"),
			textures_path("/hero/fireball/fireball_0.png"),
			textures_path("/hero/fireball/fireball_1.png"),
			textures_path("/hero/fireball/fireball_2.png"),
			textures_path("/hero/fireball/fireball_3.png"),
			textures_path("/hero/fireball/fireball_4.png"),
			textures_path("/hero/fireball/fireball_5.png"),
			textures_path("/hero/fireball/fireball_6.png"),
			textures_path("/hero/fireball/fireball_7.png"),
			textures_path("/hero/fireball/fireball_8.png"),
			textures_path("/hero/fireball/fireball_9.png"),
			textures_path("/hero/fireball/fireball_10.png"),
			textures_path("/hero/fireball/fireball_11.png"),
			textures_path("/hero/fireball/fireball_12.png"),
			textures_path("/hero/fireball/fireball_13.png"),
			textures_path("/hero/fireball/fireball_14.png"),
			textures_path("/hero/fireball/fireball_projectile.png"),
			textures_path("/hero/lightning/lightning_0.png"),
			textures_path("/hero/lightning/lightning_1.png"),
			textures_path("/hero/lightning/lightning_2.png"),
			textures_path("/hero/lightning/lightning_3.png"),
			textures_path("/hero/lightning/lightning_4.png"),
			textures_path("/hero/lightning/lightning_5.png"),
			textures_path("/hero/lightning/lightning_6.png"),
			textures_path("/hero/lightning/lightning_7.png"),
			textures_path("/hero/lightning/lightning_8.png"),
			textures_path("/hero/lightning/lightning_9.png"),
			textures_path("/hero/lightning/lightning_10.png"),
			textures_path("/hero/lightning/lightning_projectile.png"),
			textures_path("/hero/ice_shard/ice_shard_0.png"),
			textures_path("/hero/ice_shard/ice_shard_1.png"),
			textures_path("/hero/ice_shard/ice_shard_2.png"),
			textures_path("/hero/ice_shard/ice_shard_3.png"),
			textures_path("/hero/ice_shard/ice_shard_4.png"),
			textures_path("/hero/ice_shard/ice_shard_5.png"),
			textures_path("/hero/ice_shard/ice_shard_6.png"),
			textures_path("/hero/ice_shard/ice_shard_7.png"),
			textures_path("/hero/ice_shard/ice_shard_8.png"),
			textures_path("/hero/ice_shard/ice_shard_projectile.png"),
			textures_path("/hero/zap/zap_0.png"),
			textures_path("/hero/zap/zap_1.png"),
			textures_path("/hero/zap/zap_2.png"),
			textures_path("/hero/zap/zap_3.png"),
			textures_path("/hero/zap/zap_4.png"),
			textures_path("/hero/zap/zap_5.png"),
			textures_path("/hero/zap/zap_6.png"),
			textures_path("/hero/quake/quake_0.png"),
			textures_path("/hero/quake/quake_1.png"),
			textures_path("/hero/quake/quake_2.png"),
			textures_path("/hero/quake/quake_3.png"),
			textures_path("/hero/quake/quake_4.png"),
			textures_path("/hero/quake/quake_5.png"),
			textures_path("/hero/lifetap/lifetap_0.png"),
			textures_path("/hero/lifetap/lifetap_1.png"),
			textures_path("/hero/lifetap/lifetap_2.png"),
			textures_path("/hero/lifetap/lifetap_3.png"),
			textures_path("/hero/lifetap/lifetap_4.png"),
			textures_path("/hero/lifetap/lifetap_5.png"),
			textures_path("/hero/lifetap/lifetap_6.png"),
			textures_path("/hero/lifetap/lifetap_7.png"),
			textures_path("/hero/lifetap/lifetap_projectile.png"),
			textures_path("/enemies/boss/idle/boss_idle_0.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_0.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_0.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_0.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_1.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_1.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_1.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_1.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_2.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_3.png"),
			textures_path("/enemies/boss/fireball/boss_fireball_4.png"),
			textures_path("/enemies/boss/fireball/fireball.png"),
			textures_path("character_zombie_idle.png"),
			textures_path("/ui/attack_button.png"),
			textures_path("/ui/hp_bar_hero.png"),
			textures_path("/ui/hp_bar_enemy.png"),
			textures_path("/ui/player_health_icon.png"),
			textures_path("/ui/mana_icon.png"),
			textures_path("/ui/line.png"),
			textures_path("/ui/question_mark.png"),
			textures_path("/ui/tutorial.png"),
			textures_path("/ui/backgroundCastles.png"),
			textures_path("/ui/arrow.png"),
			textures_path("/ui/wall_texture.png"),
			textures_path("/ui/play_button.png"),
			textures_path("ui/mountainCombat.png"),
			textures_path("ui/hillsCombat.png"),
			textures_path("ui/background.png"),
			textures_path("/enemies/tank/idle/tank_idle_0.png"),
			textures_path("/enemies/tank/punch/tank_punch_0.png"),
			textures_path("/enemies/tank/punch/tank_punch_0.png"),
			textures_path("/enemies/tank/punch/tank_punch_0.png"),
			textures_path("/enemies/tank/punch/tank_punch_1.png"),
			textures_path("/enemies/tank/punch/tank_punch_1.png"),
			textures_path("/enemies/tank/punch/tank_punch_1.png"),
			textures_path("/enemies/tank/punch/tank_punch_1.png"),
			textures_path("/enemies/tank/punch/tank_punch_2.png"),
			textures_path("/enemies/tank/punch/tank_punch_2.png"),
			textures_path("/enemies/tank/punch/tank_punch_2.png"),
			textures_path("/enemies/tank/punch/tank_punch_2.png"),
			textures_path("/enemies/tank/punch/tank_punch_3.png"),
			textures_path("/enemies/tank/punch/tank_punch_3.png"),
			textures_path("/enemies/tank/punch/tank_punch_3.png"),
			textures_path("/enemies/tank/punch/tank_punch_4.png"),
			textures_path("/enemies/tank/punch/tank_punch_4.png"),
			textures_path("/enemies/tank/punch/tank_punch_4.png"),
			textures_path("/ui/restart_button.png"),
			textures_path("/ui/exit_button.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_0.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_0.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_0.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_1.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_1.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_1.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_1.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_2.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_2.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_2.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_2.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_3.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_3.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_3.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_4.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_4.png"),
			textures_path("/enemies/tank/heavypunch/tank_heavypunch_4.png"),
			textures_path("/enemies/healer/idle/healer_idle_0.png"),
			textures_path("/enemies/healer/heal/healer_heal_0.png"),
			textures_path("/enemies/healer/heal/healer_heal_0.png"),
			textures_path("/enemies/healer/heal/healer_heal_0.png"),
			textures_path("/enemies/healer/heal/healer_heal_1.png"),
			textures_path("/enemies/healer/heal/healer_heal_1.png"),
			textures_path("/enemies/healer/heal/healer_heal_1.png"),
			textures_path("/enemies/healer/heal/healer_heal_1.png"),
			textures_path("/enemies/healer/heal/healer_heal_2.png"),
			textures_path("/enemies/healer/heal/healer_heal_2.png"),
			textures_path("/enemies/healer/heal/healer_heal_2.png"),
			textures_path("/enemies/healer/heal/healer_heal_2.png"),
			textures_path("/enemies/healer/heal/healer_heal_3.png"),
			textures_path("/enemies/healer/heal/healer_heal_3.png"),
			textures_path("/enemies/healer/heal/healer_heal_3.png"),
			textures_path("/enemies/healer/heal/healer_heal_4.png"),
			textures_path("/enemies/healer/heal/healer_heal_4.png"),
			textures_path("/enemies/healer/heal/healer_heal_4.png"),
			textures_path("/enemies/healer/spell/healer_spell_0.png"),
			textures_path("/enemies/healer/spell/healer_spell_0.png"),
			textures_path("/enemies/healer/spell/healer_spell_0.png"),
			textures_path("/enemies/healer/spell/healer_spell_1.png"),
			textures_path("/enemies/healer/spell/healer_spell_1.png"),
			textures_path("/enemies/healer/spell/healer_spell_1.png"),
			textures_path("/enemies/healer/spell/healer_spell_1.png"),
			textures_path("/enemies/healer/spell/healer_spell_2.png"),
			textures_path("/enemies/healer/spell/healer_spell_2.png"),
			textures_path("/enemies/healer/spell/healer_spell_2.png"),
			textures_path("/enemies/healer/spell/healer_spell_2.png"),
			textures_path("/enemies/healer/spell/healer_spell_3.png"),
			textures_path("/enemies/healer/spell/healer_spell_3.png"),
			textures_path("/enemies/healer/spell/healer_spell_3.png"),
			textures_path("/enemies/healer/spell/healer_spell_4.png"),
			textures_path("/enemies/healer/spell/healer_spell_4.png"),
			textures_path("/enemies/healer/spell/healer_spell_4.png"),
			textures_path("/enemies/ranged/idle/ranged_idle_0.png"),
			textures_path("/enemies/ranged/animation/ranged_animation_0.png"),
			textures_path("/enemies/ranged/animation/ranged_animation_0.png"),
			textures_path("/enemies/ranged/animation/ranged_animation_0.png"),
			textures_path("/enemies/ranged/animation/ranged_animation_1.png"),
			textures_path("/enemies/ranged/animation/ranged_animation_1.png"),
			textures_path("/enemies/ranged/animation/ranged_animation_1.png"),
			textures_path("/enemies/ranged/projectile/ice_spear.png"),
			textures_path("/enemies/melee/idle/melee_idle.png"),
			textures_path("/enemies/melee/punch/melee_punch_0.png"),
			textures_path("/enemies/melee/punch/melee_punch_0.png"),
			textures_path("/enemies/melee/punch/melee_punch_0.png"),
			textures_path("/enemies/melee/punch/melee_punch_1.png"),
			textures_path("/enemies/melee/punch/melee_punch_1.png"),
			textures_path("/enemies/melee/punch/melee_punch_1.png"),
			textures_path("/enemies/melee/punch/melee_punch_2.png"),
			textures_path("/enemies/melee/punch/melee_punch_2.png"),
			textures_path("/enemies/melee/punch/melee_punch_2.png"),
			textures_path("/enemies/melee/punch/melee_punch_3.png"),
			textures_path("/enemies/melee/punch/melee_punch_3.png"),
			textures_path("/enemies/melee/punch/melee_punch_3.png"),
			textures_path("/enemies/melee/slash/melee_slash_0.png"),
			textures_path("/enemies/melee/slash/melee_slash_0.png"),
			textures_path("/enemies/melee/slash/melee_slash_0.png"),
			textures_path("/enemies/melee/slash/melee_slash_1.png"),
			textures_path("/enemies/melee/slash/melee_slash_1.png"),
			textures_path("/enemies/melee/slash/melee_slash_1.png"),
			textures_path("/enemies/melee/slash/melee_slash_2.png"),
			textures_path("/enemies/melee/slash/melee_slash_2.png"),
			textures_path("/enemies/melee/slash/melee_slash_2.png"),
			textures_path("/enemies/melee/slash/melee_slash_3.png"),
			textures_path("/enemies/melee/slash/melee_slash_3.png"),
			textures_path("/enemies/melee/slash/melee_slash_3.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_0.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_0.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_0.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_1.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_1.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_1.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_2.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_2.png"),
			textures_path("/enemies/melee/heavy_hit/heavy_hit_2.png")
  };





	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("egg"),
		shader_path("salmon"),
		shader_path("textured"),
		shader_path("water"),
		shader_path("fire") };

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	void renderMap();
	void renderTutorial();
	void renderGameOver();
	void renderMenu();
	void renderLoot(const std::vector<LootOption>& lootOptions);
	void updateLootTextColor(bool updateColor, const std::string& loot);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	void setGameState(GameState new_state);

	mat3 createProjectionMatrix();

	// Text rendering functions
	std::string readShaderFile(const std::string& filename);
	bool fontInit(const std::string& font_filename, unsigned int font_default_size);
	void renderText(UIText text, const glm::mat4& trans);

	GameState game_state = GameState::MENU;

	std::string selectedLoot = "";

	bool newAttackSelected = false;
	std::vector<vec4> lootBoundingBoxes;
	bool noMana = false;
	int lootHover = 9;

private:

	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawTexturedMeshMap(Entity entity, const mat3& projection);
	void drawTexturedMeshMenu(Entity entity, const mat3& projection);
	void drawToScreen();
	glm::vec2 calculateTextDimensions(const UIText& text);

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;

	// Font
	std::map<char, Character> m_ftCharacters;
	GLuint m_font_shaderProgram;
	GLuint m_font_VAO;
	GLuint m_font_VBO;
	GLuint vao; // not really used, need one to not crash (from template)

	// Animation
	double time_old = 0.0f;
	double frames_ps = 30.0f;
	GLuint handleHeroAnimation(Entity entity);
	GLuint handleEnemyAnimation(Entity entity);
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
