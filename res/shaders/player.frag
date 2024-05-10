#version 330 core

out vec4 FragColour;

in vec2 texCoords;

uniform sampler2D image;

void main()
{
    FragColour = texture(image, vec2(texCoords.x, texCoords.y));
}
