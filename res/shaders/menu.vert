#version 330 core

layout(location = 0) in vec2 pos;

out vec2 texCoords;

void main()
{
    texCoords = pos * 1.f + 0.5f; // adjust for -0.5 and 0.5
    gl_Position = vec4(pos, 0.f, 1.f);
}
