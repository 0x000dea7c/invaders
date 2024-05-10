#version 330 core

layout (location = 0) in vec4 data;

out vec2 texCoords;

void main()
{
    texCoords = data.zw;
    gl_Position = vec4(data.xy, 0.0, 1.0);
}
