#version 330 core

layout(location = 0) in mat4 model;
layout(location = 4) in vec4 vertex_data[6];
layout(location = 10) in vec4 colour;

out vec4 outColour;
out vec2 texCoords;

uniform mat4 projection;

void main()
{
  outColour = colour;
  texCoords = vertex_data[gl_VertexID].zw;
  gl_Position = projection * model * vec4(vertex_data[gl_VertexID].x + colour.w * 0.05f,
                                          vertex_data[gl_VertexID].y + colour.w * 0.05f,
                                          0.0f,
                                          1.0f);
}
