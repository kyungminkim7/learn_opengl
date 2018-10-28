#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fragTextureCoordinates;

void main(void)
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    fragTextureCoordinates = vertexTextureCoordinates;
}
