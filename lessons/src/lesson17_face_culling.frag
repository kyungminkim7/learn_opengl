#version 330 core
out vec4 fragColor;

in vec2 fragTextureCoordinates;

uniform sampler2D texture0;

void main(void) {
    fragColor = texture2D(texture0, fragTextureCoordinates);
}
