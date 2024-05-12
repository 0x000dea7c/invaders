#version 330 core

in vec2 texCoords;

out vec4 FragColour;

uniform sampler2D image;

void main()
{
  FragColour = texture(image, texCoords);
}
