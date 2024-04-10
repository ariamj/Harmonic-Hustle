// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

#include <glm/gtc/type_ptr.hpp>
#include "iostream"
// stlib
#include <chrono>
using Clock = std::chrono::high_resolution_clock;

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	// Handle transformations and rendering
	// For references, refer back to A1 template code
	Motion &motion = registry.motions.get(entity);
	// Transformation code
	Transform transform;
	transform.translate(motion.position);
	transform.scale(motion.scale * motion.scale_factor);
	transform.rotate(motion.angle);

	assert(registry.renderRequests.has(entity));
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

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::PLAYER || render_request.used_effect == EFFECT_ASSET_ID::ENEMY_GUITAR)
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
		gl_has_errors();

		// if (render_request.used_effect == EFFECT_ASSET_ID::PLAYER)
		// {
		// 	// Light up?
		// 	// GLint light_up_uloc = glGetUniformLocation(program, "light_up");
		// 	// assert(light_up_uloc >= 0);

		// 	// GLint light_up = registry.lightUps.has(entity) ? 1 : 0;
		// 	// glUniform1i(light_up_uloc, light_up);
		// 	// gl_has_errors();
		// }
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colours.has(entity) ? registry.colours.get(entity) : vec3(1);
	// const vec3 color = vec3(1);
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

// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen()
{
	// handle drawing to screen
	// For references, refer back to A1 template code
	// Setting shaders
	// get the wind texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::ENVIRONMENT]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	// glClearColor(0.032f, 0.139f, 0.153f, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint env_program = effects[(GLuint)EFFECT_ASSET_ID::ENVIRONMENT];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(env_program, "time");
	GLuint darken_screen_uloc = glGetUniformLocation(env_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(darken_screen_uloc, screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(env_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	Screen curr_screen = registry.screens.get(screen_state_entity);
 	if (curr_screen == OVERWORLD || curr_screen == START || curr_screen == OPTIONS) {
		GLuint texture_id =
			texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::OVERWORLD_BG];
		glBindTexture(GL_TEXTURE_2D, texture_id);

	} else if (curr_screen == SETTINGS) {
		GLuint texture_id =
			texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::HELP_BG];
		glBindTexture(GL_TEXTURE_2D, texture_id);
	} else if (curr_screen == TUTORIAL) {
		GLuint texture_id =
			texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::OVERWORLD_BG];
		glBindTexture(GL_TEXTURE_2D, texture_id);
		
	} else {
		glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	}
	
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, // changed from 3 for elements
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

void RenderSystem::renderText(const std::string& text, float x, float y,
		float scale, const glm::vec3& color,
		const glm::mat4& trans, bool center_pos) {

	// activate the shaders!
	glUseProgram(m_font_shaderProgram);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int textColor_location =
		glGetUniformLocation(
			m_font_shaderProgram,
			"textColor"
		);
	assert(textColor_location >= 0);
	glUniform3f(textColor_location, color.x, color.y, color.z);

	auto transform_location = glGetUniformLocation(
		m_font_shaderProgram,
		"transform"
	);
	assert(transform_location > -1);
	glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(trans));

	glBindVertexArray(m_font_VAO);

	scale = scale * 0.85;

	float textWidth = 0.f;
	float textHeight = 0.f;
	std::string::const_iterator text_c;
	for (text_c = text.begin(); text_c != text.end(); text_c++) {
		Character text_ch = m_ftCharacters[*text_c];
		textWidth += text_ch.Advance >> 6;
		textHeight = max(textHeight, (float)text_ch.Size.y);
	}
	if (center_pos) {
		// Adjust xpos to be center of text
		x -= textWidth * scale / 2.f;
	}
	y = gameInfo.height - y - ((textHeight / 2.f) * scale);

	// Build an array of character-specific data
	CharacterRequest character_requests[MAX_TEXT_LENGTH];

	// iterate through all characters
	std::string::const_iterator c;
	int i = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		CharacterRequest character_request;

		Character ch = m_ftCharacters[*c];
		character_request.TextureID = ch.TextureID;

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		// update VBO for each character
		// float vertices[6][4] = {
		// 	{ xpos,     ypos + h,   0.0f, 0.0f },
		// 	{ xpos,     ypos,       0.0f, 1.0f },
		// 	{ xpos + w, ypos,       1.0f, 1.0f },

		// 	{ xpos,     ypos + h,   0.0f, 0.0f },
		// 	{ xpos + w, ypos,       1.0f, 1.0f },
		// 	{ xpos + w, ypos + h,   1.0f, 0.0f }
		// };

		// Assignment operator didn't work... there must be a better way to do this
		character_request.vertices[0][0] = xpos;
		character_request.vertices[0][1] = ypos + h;
		character_request.vertices[0][2] = 0.0f;
		character_request.vertices[0][3] = 0.0f;
		character_request.vertices[1][0] = xpos;
		character_request.vertices[1][1] = ypos;
		character_request.vertices[1][2] = 0.0f;
		character_request.vertices[1][3] = 1.0f;
		character_request.vertices[2][0] = xpos + w;
		character_request.vertices[2][1] = ypos;
		character_request.vertices[2][2] = 1.0f;
		character_request.vertices[2][3] = 1.0f;
		character_request.vertices[3][0] = xpos;
		character_request.vertices[3][1] = ypos + h;
		character_request.vertices[3][2] = 0.0f;
		character_request.vertices[3][3] = 0.0f;
		character_request.vertices[4][0] = xpos + w;
		character_request.vertices[4][1] = ypos;
		character_request.vertices[4][2] = 1.0f;
		character_request.vertices[4][3] = 1.0f;
		character_request.vertices[5][0] = xpos + w;
		character_request.vertices[5][1] = ypos + h;
		character_request.vertices[5][2] = 1.0f;
		character_request.vertices[5][3] = 0.0f;

		character_requests[i] = character_request;
		
		i += 1;
		x += (ch.Advance >> 6) * scale;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
	glBufferData(GL_ARRAY_BUFFER, text.size() * sizeof(CharacterRequest), character_requests, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

	for (int i = 0; i < text.size(); i++) {
		// Manage a pointer instead of calling glSubBufferData repeatedly
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)(i * sizeof(CharacterRequest)));
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, character_requests[i].TextureID);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Rebind VAO
	glBindVertexArray(vao);
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// handle drawing game
	// For references, refer back to A1 template code
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	// glClearColor(0.032, 0.139, 0.153, 1.0); // background colour
	// glClearColor(0.048, 0.184, 0.201, 1.0); // background colour
	vec3 bckgd_colour = Colour::theme_blue_3;
	glClearColor(bckgd_colour.x, bckgd_colour.y, bckgd_colour.z, 1.0); // background colour
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	// Draw all textured meshes that have a position and size component
	// for (Entity entity : registry.renderRequests.entities)
	// {
	// 	// if (!registry.motions.has(entity))
	// 	// 	continue;
	// 	// Note, its not very efficient to access elements indirectly via the entity
	// 	// albeit iterating through all Sprites in sequence. A good point to optimize
	// 	drawTexturedMesh(entity, projection_2D);
	// }

	glBindVertexArray(vao);

	drawToScreen();

	// Mesh rendering - background entities
	Screen curr_screen = registry.screens.get(screen_state_entity);
	for (Entity entity : registry.renderRequests.entities)
	{
		// render entity only if belongs to same screen as screen_state_entity
		if (registry.screens.has(entity)) {
			Screen entity_screen = registry.screens.get(entity);
			if (entity_screen == curr_screen && registry.backgrounds.has(entity)) {
				drawTexturedMesh(entity, projection_2D);
			}
		}
	}

	// Mesh rendering
	for (Entity entity : registry.renderRequests.entities)
	{
		// render entity only if belongs to same screen as screen_state_entity
		if (registry.screens.has(entity)) {
			Screen entity_screen = registry.screens.get(entity);
			if (entity_screen == curr_screen && !registry.backgrounds.has(entity) && !registry.foregrounds.has(entity)) {
				drawTexturedMesh(entity, projection_2D);
			}
		}
	}

	// Don't render particles in options
	if (!gameInfo.in_options) {
		// Particle rendering, between foreground and background layers
		for (auto generator : particle_generators) {
				generator->Draw();
		}
	}
	glBindVertexArray(vao);

	// Mesh rendering - foreground entities
	for (Entity entity : registry.renderRequests.entities)
	{
		// render entity only if belongs to same screen as screen_state_entity
		if (registry.screens.has(entity) && registry.foregrounds.has(entity)) {
			Screen entity_screen = registry.screens.get(entity);
			if (entity_screen == curr_screen) {
				drawTexturedMesh(entity, projection_2D);
			}
		}
	}


	// Font matrix transformation
	// glm::mat4 trans = glm::mat4(1.0f);

	// Font matrix rotation
	// trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	// trans = glm::scale(trans, glm::vec3(0.25, 0.25, 1.0));
	// trans = glm::mat4(1.0f);
	// trans = glm::rotate(trans, glm::radians(window.rotation), glm::vec3(0.0, 0.0, 1.0));
	// trans = glm::scale(trans, glm::vec3(0.5, 0.5, 1.0));

	// Text-rendering
	for (Entity entity : registry.texts.entities) {
		Text text_e = registry.texts.get(entity);
		if (text_e.screen == curr_screen) {
			renderText(text_e.text, text_e.position.x, text_e.position.y, text_e.scale, text_e.colour, text_e.trans, text_e.center_pos);
		}
	}

	// Truely render to the screen
	// flicker-free display with a double buffer
	// glfwSwapInterval(0) // Disable VSync which improved FPS-based profiling, but didn't actually improve performance
	glfwSwapBuffers(window); // 
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	// float right = (float) window_width_px;
	// float bottom = (float) window_height_px;
	float right = (float) gameInfo.width;
	float bottom = (float) gameInfo.height;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

void RenderSystem::createParticleGenerator(int particle_type_id, int additional_particles) {
	switch (particle_type_id) {
		case (int)PARTICLE_TYPE_ID::TRAIL:
		{
			GLuint shaderProgram = effects[(GLuint)EFFECT_ASSET_ID::TRAIL_PARTICLE];
			GLuint usedTexture = texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::TRAIL_PARTICLE];
			int amount = 400;
			int max_entities = 10;
			std::shared_ptr<TrailParticleGenerator> generator =
				std::make_shared<TrailParticleGenerator>(TrailParticleGenerator(shaderProgram, usedTexture, amount, max_entities));
			particle_generators.push_back(generator);
			generator->setAdditionalParticles(additional_particles);
			return;
		}
		case (int)PARTICLE_TYPE_ID::SMOKE:
		{
			GLuint shaderProgram = effects[(GLuint)EFFECT_ASSET_ID::SMOKE_PARTICLE];
			GLuint usedTexture = texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::SMOKE_PARTICLE];
			int amount = 120;
			int max_entities = 30;
			std::shared_ptr<SmokeParticleGenerator> generator =
				std::make_shared<SmokeParticleGenerator>(SmokeParticleGenerator(shaderProgram, usedTexture, amount, max_entities));
			particle_generators.push_back(generator);
			return;
		}
		case (int)PARTICLE_TYPE_ID::FLAME:
		{
			GLuint shaderProgram = effects[(GLuint)EFFECT_ASSET_ID::FLAME_PARTICLE];
			GLuint usedTexture = texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::FLAME_PARTICLE];
			int amount = 400;
			int max_entities = 15;
			std::shared_ptr<FlameParticleGenerator> generator =
				std::make_shared<FlameParticleGenerator>(FlameParticleGenerator(shaderProgram, usedTexture, amount, max_entities));
			particle_generators.push_back(generator);
			generator->setAdditionalParticles(additional_particles);
			return;
		}
		case (int)PARTICLE_TYPE_ID::SPARK:
		{
			GLuint shaderProgram = effects[(GLuint)EFFECT_ASSET_ID::SPARK_PARTICLE];
			GLuint usedTexture = texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::SPARK_PARTICLE];
			int amount = 150;
			int max_entities = 4;
			std::shared_ptr<SparkParticleGenerator> generator =
				std::make_shared<SparkParticleGenerator>(SparkParticleGenerator(shaderProgram, usedTexture, amount, max_entities));
			particle_generators.push_back(generator);
			return;
		}
	}
}