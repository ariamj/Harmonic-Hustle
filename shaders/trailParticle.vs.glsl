// https://learnopengl.com/In-Practice/2D-Game/Particles

#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 aOffset;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aLife;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
// uniform vec2 offset;
// uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = vertex.zw;
    ParticleColor = aColor;
    if (aLife < 0.f)
        ParticleColor = vec4(0.f, 0.f, 0.f, 0.f);
    gl_Position = projection * vec4((vertex.xy * scale) + aOffset, 0.0, 1.0);
}