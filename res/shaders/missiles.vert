#version 330 core

layout(location = 0) in mat4 model;
layout(location = 4) in vec4 vertexData[6];

out vec2 texCoords;

uniform mat4 projection; // TODO: try use UBO maybe?

void main()
{
    texCoords = vertexData[gl_VertexID].zw;
    gl_Position = projection * model * vec4(vertexData[gl_VertexID].xy, 0.f, 1.0f);
}
