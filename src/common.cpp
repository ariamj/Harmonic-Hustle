#include "common.hpp"
#include <random>

int FPS = 60;

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
		assert(false);
	}

	return true;
}

vec2 getRandomEnemyVelocity() {
	std::default_random_engine rng = std::default_random_engine(std::random_device()());
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	float leftOrRight = uniform_dist(rng) <= 0.5 ? 1.0f : -1.0f;
	float upOrDown = uniform_dist(rng) <= 0.5 ? 1.0f : -1.0f;
	float partVelocity = uniform_dist(rng);

	return {leftOrRight * partVelocity * DEFAULT_ENEMY_VELOCITY, upOrDown * (1.0f - partVelocity) * DEFAULT_ENEMY_VELOCITY};
}


// From https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/a-brief-introduction-to-lerp-r4954/
// Linked on Canvas M1 requirements
float lerp(float start, float end, float t) {
	return start * (1 - t) + end * t;
}