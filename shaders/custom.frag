// This fragment shader is intented to be customized as you wish

// DO NOT DELETE
#version 460 core

in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D screenTexture;
uniform float time;

void main()
{
    fragColor = vec4(texture(screenTexture, texCoords).rgb, 1.0f);
}