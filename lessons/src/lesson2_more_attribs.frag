#version 330 core
out vec4 FragColor;
in vec3 fragInColor;

void main(void)
{
    FragColor = vec4(fragInColor, 1.0);
}
