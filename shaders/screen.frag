#version 460 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;

void main()
{
    // Normal 
    fragColor = vec4(texture(screenTexture, texCoords).rgb, 1.0f);
}