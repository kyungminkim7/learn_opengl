#version 330 core
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 offset;

out vec3 fragInColor;

void main(void)
{
    gl_Position = vec4(vertexPosition * gl_InstanceID / 100.0 + offset,
                       0.0, 1.0);
    fragInColor = vertexColor;
}
