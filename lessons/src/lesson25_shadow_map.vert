#version 330 core
layout (location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main(void) {
    gl_Position = lightSpaceMatrix * model * vec4(vertexPosition, 1.0);
}
