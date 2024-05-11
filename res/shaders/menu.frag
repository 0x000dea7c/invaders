#version 330 core

in vec2 texCoords;

out vec4 FragColour;

const float edge_width = 0.05f;

void main()
{
    float alpha = 1.0 - smoothstep(1.0 - edge_width, 1.0, max(texCoords.x, texCoords.y));
    alpha = min(alpha, 1.0 - smoothstep(1.0 - edge_width, 1.0, max(1.0 - texCoords.x, 1.0 - texCoords.y)));

    FragColour = vec4(0.f, 0.f, 0.f, .1f * alpha);
}
