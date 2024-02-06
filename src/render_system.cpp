// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	// Handle transformations and rendering
	// For references, refer back to A1 template code
}

// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen()
{
	// handle drawing to screen
	// For references, refer back to A1 template code
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// handle drawing game
	// For references, refer back to A1 template code
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