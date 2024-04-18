#version 330 core

layout(location = 0) in vec4 data;

out vec2 texCoords;

uniform mat4 projection;
uniform mat4 model;

void main()
{
    texCoords = data.zw;
    gl_Position = projection * model * vec4(data.xy, 0.f, 1.f);
}
