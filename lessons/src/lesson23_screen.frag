#version 330 core
uniform sampler2D screenTexture;

out vec4 fragColor;

in vec2 fragTextureCoordinates;

void main()
{
    fragColor = texture2D(screenTexture, fragTextureCoordinates);
}
