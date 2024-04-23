#version 330 core

in vec2 texCoords;

out vec4 FragColour;

uniform sampler2D image;
uniform float time;

void main()
{
    vec2 tex = texCoords;
    tex.y += time;
    FragColour = texture(image, tex);
}
