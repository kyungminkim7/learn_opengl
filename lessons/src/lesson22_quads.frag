#version 330 core
in vec3 fragInColor;

out vec4 fragOutColor;

void main()
{
    fragOutColor = vec4(fragInColor, 1.0);
}
