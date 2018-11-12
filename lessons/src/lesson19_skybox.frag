#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
out vec4 fragColor;

in vec3 fragTextureCoordinates;

uniform samplerCube skybox;

void main(void) {
    fragColor = textureCube(skybox, fragTextureCoordinates);
}
