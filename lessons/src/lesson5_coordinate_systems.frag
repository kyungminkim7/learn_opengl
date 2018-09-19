#version 330 core
out vec4 FragColor;

in vec2 fragTextureCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float texMix;

void main(void)
{
    FragColor = mix(texture2D(texture0, fragTextureCoord),
                    texture2D(texture1, fragTextureCoord),
                    texMix);
}
