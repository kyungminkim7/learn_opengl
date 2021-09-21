#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
    fragColor = mix(texture2D(texture0, texCoord),
                    texture2D(texture1, texCoord), 0.2);
}
