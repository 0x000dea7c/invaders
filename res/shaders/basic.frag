#version 330 core

in vec2 texCoords;

out vec4 FragColour;

uniform sampler2D image;
uniform float alpha;

void main()
{
  FragColour = texture(image, texCoords);
  FragColour.a = alpha;
}
