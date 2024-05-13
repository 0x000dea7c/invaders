#version 330 core

out vec4 FragColour;

in vec2 texCoords;

uniform sampler2D image;
uniform vec4 textColour;

void main()
{
    vec4 sampled = vec4(1.f, 1.f, 1.f, texture(image, texCoords).r);
    FragColour = sampled * textColour;
}
