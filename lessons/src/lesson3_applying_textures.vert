#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 fragInColor;
out vec2 fragInTexCoord;

void main(void)
{
    gl_Position = vec4(pos, 1.0);
    fragInColor = color;
    fragInTexCoord = texCoord;
}
