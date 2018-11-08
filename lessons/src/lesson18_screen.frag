#version 330 core
out vec4 fragColor;

in vec2 fragTextureCoordinates;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main(void) {
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0f, offset),
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(offset, 0.0f),
        vec2(-offset, -offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
    );

    float kernel[9] = float[](
         1, 1, 1,
         1, -8, 1,
         1, 1, 1
    );

    vec3 sampleTexture[9];
    for (int i = 0; i < 9; ++i) {
        sampleTexture[i] = vec3(texture2D(screenTexture, fragTextureCoordinates.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; ++i) {
        color += sampleTexture[i] * kernel[i];
    }

    fragColor = vec4(color, 1.0);
}
