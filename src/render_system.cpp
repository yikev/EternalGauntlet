// internal
#include "render_system.hpp"
#include "world_system.hpp"
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "loot.hpp" 

#include "tiny_ecs_registry.hpp"

void RenderSystem::renderMap(){
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // Set a dark blue background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers
	mat3 projection_2D = createProjectionMatrix();
	Entity e = registry.uis.entities[5];
	drawTexturedMesh(e, projection_2D);
}

void RenderSystem::renderGameOver() {

	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat3 projection_2D = createProjectionMatrix();

	UIText gameOverText = {
            UI_TYPE::UI_BASIC,
            {}, 
            "Game Over!",
            vec2(window_width_px / 2 - 500.f, window_height_px / 2), 
            1.5f, 
        vec3(1.f, 0.f, 0.f) // Red color
    };

    renderText(gameOverText, glm::mat4(1.0f));

	UIText restartText = {
        UI_TYPE::UI_BASIC,
        {}, 
        "You ran out of mana, Press 'R' to restart",
        vec2(window_width_px / 2 - 500.f, window_height_px / 2 - 40.f), 
        0.8f, 
        vec3(1.f, 1.f, 1.f) // White color
    };

	UIText deathText = {
        UI_TYPE::UI_BASIC,
        {}, 
        "You died, Press 'R' to restart",
        vec2(window_width_px / 2 - 500.f, window_height_px / 2 - 40.f), 
        0.8f, 
        vec3(1.f, 1.f, 1.f) // White color
    };

	if (noMana) {
		renderText(restartText, glm::mat4(1.0f));
	} else {
		renderText(deathText, glm::mat4(1.0f));
	}
}

void RenderSystem::renderTutorial() {
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // Set a dark blue background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers
	mat3 projection_2D = createProjectionMatrix();
	Entity entity = registry.uis.entities[4];
	drawTexturedMesh(entity, projection_2D);
}

void RenderSystem::renderMenu() {
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);  // Set a sky blue background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers
	mat3 projection_2D = createProjectionMatrix();

	// Render entities
    Entity entity1 = registry.uis.entities[5];
    drawTexturedMesh(entity1, projection_2D);

    Entity entity2 = registry.uis.entities[6];
    drawTexturedMesh(entity2, projection_2D);

    Entity entity3 = registry.uis.entities[7];
    drawTexturedMesh(entity3, projection_2D);

	Entity entity4 = registry.uis.entities[8];
    drawTexturedMesh(entity4, projection_2D);

	Entity entity5 = registry.uis.entities[9];
    drawTexturedMesh(entity5, projection_2D);

	// Render text over entities
    auto renderEntityText = [&](const Entity& entity, const std::string& text, const vec3& color) {
        vec2 entityPosition = registry.uis.get(entity).position;  // Assuming entities have a Transform component
        UIText entityText = {
            UI_TYPE::UI_BASIC,
            {},
            text,
            vec2(entityPosition.x - 50, entityPosition.y - 350),  // Position text slightly above the entity
            0.7f,  // Scale of text
            color   // Text color
        };
        glDisable(GL_DEPTH_TEST);  // Ensure text renders over entities
        renderText(entityText, glm::mat4(1.0f));
        glEnable(GL_DEPTH_TEST);   // Re-enable depth testing for other rendering
    };

    // Render text labels for entities
    renderEntityText(entity2, "PLAY", vec3(0.f, 0.f, 0.f)); 
    renderEntityText(entity3, "", vec3(0.f, 0.f, 0.f));  
    renderEntityText(entity4, "RESTART", vec3(0.f, 0.f, 0.f));  
	renderEntityText(entity5, "EXIT GAME", vec3(0.f, 0.f, 0.f));

	UIText nameText = {
            UI_TYPE::UI_BASIC,
            {},
			"ETERNAL GAUNTLET",
            vec2(window_width_px / 2 - 550, window_height_px / 2 + 300),
            2.0f,
            vec3(0.f,0.f,0.f)
        };
        renderText(nameText, glm::mat4(1.0f));


}

void RenderSystem::renderLoot(const std::vector<LootOption>& lootOptions) {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat3 projection_2D = createProjectionMatrix();

    Entity entity = registry.uis.entities[3];
    drawTexturedMesh(entity, projection_2D);

    // Render the title
    UIText titleText = {
        UI_TYPE::UI_BASIC,
        {}, // No linked entity
        "Choose your reward!",
        vec2(window_width_px / 2 - 250.f, window_height_px - 50.f), // Position near the top
        1.0f, // Scale for large text
        vec3(1.f, 1.f, 0.f) // Yellow color
    };
    renderText(titleText, glm::mat4(1.0f));

    lootBoundingBoxes.clear(); 

    float yOffset = 100.f;

    for (size_t i = 0; i < lootOptions.size(); ++i) {
        const LootOption& loot = lootOptions[i];
        vec3 color = (loot.name == selectedLoot) ? vec3(1.f, 1.f, 0.f) : vec3(1.f, 1.f, 1.f); // Yellow if selected
		if (lootHover == i) {
			color =  vec3(0.f, 1.f, 0.f);
		}

        UIText nameText = {
            UI_TYPE::UI_BASIC,
            {},
            loot.name,
            vec2(window_width_px / 2, window_height_px / 2 + i * yOffset),
            0.6f,
            color
        };
        vec2 nameDimensions = calculateTextDimensions(nameText);

        UIText descText = {
            UI_TYPE::UI_BASIC,
            {},
            loot.description,
            vec2(window_width_px / 2, window_height_px / 2 + i * yOffset - 40.f),
            0.4f,
            vec3(0.8f, 0.8f, 0.8f) // Description color
        };
        vec2 descDimensions = calculateTextDimensions(descText);

        float boundingBoxWidth = std::max(nameDimensions.x, descDimensions.x);
        float boundingBoxHeight = nameDimensions.y + descDimensions.y;

        float boundingBoxX = window_width_px / 2 - boundingBoxWidth / 2;
        float boundingBoxY = window_height_px / 2 + i * yOffset - nameDimensions.y;

        lootBoundingBoxes.push_back(vec4(boundingBoxX + 200.f, boundingBoxY - 30.f, boundingBoxWidth, boundingBoxHeight+50.f));

        renderText(nameText, glm::mat4(1.0f));

        renderText(descText, glm::mat4(1.0f));
    }

    // Handle rendering for attack replacement UI
    if (newAttackSelected) {
        for (Entity entity : registry.uis.entities) {
            UI& ui = registry.uis.get(entity);
            if (ui.type == UI_TYPE::UI_ATTACK_BUTTON) {
                drawTexturedMesh(entity, projection_2D);
            }
        }

        glm::mat4 font_trans = glm::mat4(1.0f);
        for (UIText text : registry.texts.components) {
            renderText(text, font_trans);
        }

        UIText helloText = {
            UI_TYPE::UI_BASIC,
            {},
            "Choose an attack to replace.", 
            vec2(window_width_px / 2 - 300.f, window_height_px - 550.f),
            0.7f, 
            vec3(1.f, 1.f, 0.f) // Yellow color
        };
        renderText(helloText, glm::mat4(1.0f));
    }
}

vec2 RenderSystem::calculateTextDimensions(const UIText& text) {
    float width = 0.f;
    float height = 0.f;

    for (const char& c : text.text) {
        if (m_ftCharacters.find(c) != m_ftCharacters.end()) {
            const Character& ch = m_ftCharacters.at(c);

            width += (ch.Advance >> 6) * text.scale;
            height = std::max(height, ch.Size.y * text.scale); 
        }
    }

    return vec2(width, height);
}

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	// Handle transformations
	Transform transform;

	if (registry.uis.has(entity)) { // for rendering UI components
		UI &ui = registry.uis.get(entity);
		transform.translate(ui.position);
		transform.scale(ui.scale);
	} else { // for rendering regular entities
		Motion &motion = registry.motions.get(entity);
		transform.translate(motion.position);
		transform.scale(motion.scale);
		transform.rotate(motion.angle);
	}

	// Init rendering variables
	assert(registry.renderRequests.has(entity) || registry.renderRequestsMap.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		// Handle animations that use different individual textures rather than a spritesheet
		double time_now = glfwGetTime();
		double time_delta = time_now - time_old;
		if (time_delta >= 1.0f / frames_ps) {

			// Hero animations
			if (registry.players.has(entity) && registry.healths.get(entity).hp > 0) {
				texture_id = handleHeroAnimation(entity);
			}

			// Enemy animations
			if (registry.enemies.has(entity) && registry.healths.get(entity).hp > 0) {
				texture_id = handleEnemyAnimation(entity);
			}
		}
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	
		// Handle combat hit effect
		GLint hit_uloc = glGetUniformLocation(program, "hit");
		int i = registry.hits.get(entity).hit;
		glUniform1i(hit_uloc, i);
		gl_has_errors();

	} else if (render_request.used_effect == EFFECT_ASSET_ID::SALMON)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));

		GLint hit_uloc = glGetUniformLocation(program, "hit");
		int i = registry.hits.get(entity).hit;
		glUniform1i(hit_uloc, i);
		gl_has_errors();
	} else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);

	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawTexturedMeshMap(Entity entity,
	const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;

	if (registry.uis.has(entity)) { // for rendering UI components
		UI& ui = registry.uis.get(entity);
		transform.translate(ui.position);
		transform.scale(ui.scale);
	}
	else { // for rendering regular entities
		Motion& motion = registry.motions.get(entity);
		transform.translate(motion.position);
		transform.scale(motion.scale);
		transform.rotate(motion.angle);
	}

	assert(registry.renderRequests.has(entity) || registry.renderRequestsMap.has(entity));
	const RenderRequest& render_request = registry.renderRequestsMap.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequestsMap.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequestsMap.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::SALMON)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));

		GLint hit_uloc = glGetUniformLocation(program, "hit");
		int i = registry.hits.get(entity).hit;
		glUniform1i(hit_uloc, i);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);

	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WATER]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::WATER];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(water_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// render_system.cpp
void RenderSystem::setGameState(GameState new_state) {
    game_state = new_state;
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	glm::mat4 font_trans = glm::mat4(1.0f);
	// Draw all textured meshes that have a position and size component
	
	if (game_state == GameState::PLAYING) {
		for (int i = 10; i < registry.renderRequests.entities.size(); i++) {
			Entity entity = registry.renderRequests.entities[i];
            if (!registry.motions.has(entity) && !registry.uis.has(entity)) continue;
            drawTexturedMesh(entity, projection_2D);
        }
		// Draw all texts
		for (UIText text : registry.texts.components) {
			renderText(text, font_trans);
		}

		for (UIText text : registry.combatTutorialTexts.components) {
			renderText(text, font_trans);
		}
	} else if (game_state == GameState::MAP) {
		renderMap();  // renderMap is called only when in MAP state
		for (Entity entity : registry.renderRequestsMap.entities) {
			drawTexturedMeshMap(entity, projection_2D);
			renderText(registry.texts.components[18], glm::mat4(1.0f));
		}
		for (UIText text : registry.mapTutorialTexts.components) {
			renderText(text, font_trans);
		}
	} else if (game_state == GameState::TUTORIAL) {
		renderTutorial();
		renderText(registry.texts.components[18], glm::mat4(1.0f));
	} else if (game_state == GameState::LOOT) {
		renderLoot(WorldSystem::currentLootOptions);
		renderText(registry.texts.components[18], glm::mat4(1.0f));
	} else if (game_state == GameState::MENU) {
		renderMenu();
		renderText(registry.texts.components[18], glm::mat4(1.0f));
	} else if (game_state == GameState::GAMEOVER) {
		renderGameOver();
	}


	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float) window_width_px;
	float bottom = (float) window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

void RenderSystem::renderText(UIText text, const glm::mat4& trans)
	{	
		switch (text.type) {
			case UI_TYPE::UI_HEALTH:
				if (registry.healths.has(text.link)) {
					text.text = std::to_string(registry.healths.get(text.link).hp) + " / " + std::to_string(registry.healths.get(text.link).max_hp);
				}
				break;
			
			case UI_TYPE::UI_MANA:
				if (registry.manas.has(text.link)) {
					text.text = std::to_string(registry.manas.get(text.link).currentMana) + " / " + std::to_string(registry.manas.get(text.link).maxMana);
				}
				break;
			case UI_TYPE::UI_FPS:
				if (registry.fpss.has(text.link)) {
					text.text = std::to_string(registry.fpss.get(text.link).fps);
				}
				break;
			
			default:
				break;
		}

		// activate the shader program
		glUseProgram(m_font_shaderProgram);
		gl_has_errors();

		// get shader uniforms
		GLint textColor_location =
			glGetUniformLocation(m_font_shaderProgram, "textColor");
		glUniform3f(textColor_location, text.color.x, text.color.y, text.color.z);
		gl_has_errors();

		GLint transformLoc =
			glGetUniformLocation(m_font_shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		gl_has_errors();

		glBindVertexArray(m_font_VAO);
		gl_has_errors();

		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.text.begin(); c != text.text.end(); c++)
		{
			Character ch = m_ftCharacters[*c];

			float xpos = text.position.x + ch.Bearing.x * text.scale;
			float ypos = text.position.y - (ch.Size.y - ch.Bearing.y) * text.scale;

			float w = ch.Size.x * text.scale;
			float h = ch.Size.y * text.scale;
			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};

			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// std::cout << "binding texture: " << ch.character << " = " << ch.TextureID << std::endl;

			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			gl_has_errors();

			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			gl_has_errors();

			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			text.position.x += (ch.Advance >> 6) * text.scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(vao);
		gl_has_errors();
	}

GLuint RenderSystem::handleHeroAnimation(Entity entity)
{
	Player hero = registry.players.get(entity);
	int texture_asset_id = (int) registry.renderRequests.get(entity).used_texture + 1;

	if (hero.anim_type == AnimationType::IDLE) {
		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
		}
					
	} else if (hero.anim_type == AnimationType::BASIC) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_BASIC_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_BASIC_7) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
			registry.players.get(entity).anim_type = AnimationType::IDLE;
		}
	
	} else if (hero.anim_type == AnimationType::HERO_MELEE) {
		// hacky way to do "melee" attacks:
		// set hero texture to empty because the hero will now be a projectile,
		// projectile will use the correct melee attack texture (handled in world_init.cpp),
		// upon collision the world system will set the hero anim back to idle
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3 || texture_asset_id > (int) TEXTURE_ASSET_ID::EMPTY) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::EMPTY;
		}
	} else if (hero.anim_type == AnimationType::FIREBALL) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_FIREBALL_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_FIREBALL_14) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
			registry.players.get(entity).anim_type = AnimationType::IDLE;
		}
	} else if (hero.anim_type == AnimationType::LIGHTNING) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_LIGHTNING_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_LIGHTNING_9) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
			registry.players.get(entity).anim_type = AnimationType::IDLE;
		}
	} else if (hero.anim_type == AnimationType::ICESHARD) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_ICESHARD_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_ICESHARD_8) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
			registry.players.get(entity).anim_type = AnimationType::IDLE;
		}
	} else if (hero.anim_type == AnimationType::ZAP) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_ZAP_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_ZAP_6) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
			registry.players.get(entity).anim_type = AnimationType::IDLE;
		}
	} else if (hero.anim_type == AnimationType::QUAKE) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_QUAKE_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_QUAKE_5) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
			registry.players.get(entity).anim_type = AnimationType::IDLE;
		}
	} else if (hero.anim_type == AnimationType::LIFETAP) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::HERO_IDLE_3) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_LIFETAP_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::HERO_LIFETAP_7) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::HERO_IDLE_0;
			registry.players.get(entity).anim_type = AnimationType::IDLE;
		}
	}

	registry.renderRequests.get(entity).used_texture = static_cast<TEXTURE_ASSET_ID>(texture_asset_id);
	return texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];
}

GLuint RenderSystem::handleEnemyAnimation(Entity entity)
{
	Enemy enemy = registry.enemies.get(entity);
	int texture_asset_id = (int) registry.renderRequests.get(entity).used_texture + 1;

	if (enemy.anim_type == AnimationType::IDLE) {
		// TODO: no idle animation yet, for now just keep texture the same
		texture_asset_id -= 1;

	} else if (enemy.anim_type == AnimationType::FIREBALL) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int) TEXTURE_ASSET_ID::BOSS) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::BOSS_FIREBALL_0;
		}

		if (texture_asset_id > (int) TEXTURE_ASSET_ID::BOSS_FIREBALL_4) {
			texture_asset_id = (int) TEXTURE_ASSET_ID::BOSS;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}
	}
	else if (enemy.anim_type == AnimationType::PUNCH) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::TANK) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::TANK_PUNCH_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::TANK_PUNCH_4) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::TANK;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}
	}
	else if (enemy.anim_type == AnimationType::HEAVYPUNCH) {
		// start using right texture for animation from idle state
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::TANK_HEAVYPUNCH_0) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::TANK_HEAVYPUNCH_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::TANK_HEAVYPUNCH_4) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::TANK;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}
	}
	else if (enemy.anim_type == AnimationType::HEAL) {
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::HEALER_HEAL_0) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::HEALER_HEAL_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::HEALER_HEAL_4) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::HEALER;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}
	}
	else if (enemy.anim_type == AnimationType::SPELL) {
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::HEALER_SPELL_0) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::HEALER_SPELL_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::HEALER_SPELL_4) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::HEALER;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}
	}

	else if (enemy.anim_type == AnimationType::RANGED) {
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::RANGED_ANIMATION_0) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::RANGED_ANIMATION_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::RANGED_ANIMATION_1_2) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::RANGED;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}

	} else if (enemy.anim_type == AnimationType::MELEE_PUNCH) {
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::MELEE_PUNCH_0) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::MELEE_PUNCH_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::MELEE_PUNCH_3_2) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::MELEE;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}

	} else if (enemy.anim_type == AnimationType::SLASH) {
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::MELEE_SLASH_0) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::MELEE_SLASH_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::MELEE_SLASH_3_2) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::MELEE;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}
	} else if (enemy.anim_type == AnimationType::HEAVY_HIT) {
		if (texture_asset_id <= (int)TEXTURE_ASSET_ID::MELEE_HEAVY_HIT_0) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::MELEE_HEAVY_HIT_0;
		}

		if (texture_asset_id > (int)TEXTURE_ASSET_ID::MELEE_HEAVY_HIT_2_2) {
			texture_asset_id = (int)TEXTURE_ASSET_ID::MELEE;
			registry.enemies.get(entity).anim_type = AnimationType::IDLE;
		}
	}

	registry.renderRequests.get(entity).used_texture = static_cast<TEXTURE_ASSET_ID>(texture_asset_id);
	return texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];
}