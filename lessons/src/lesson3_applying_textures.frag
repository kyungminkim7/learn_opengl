#version 330 core
out vec4 FragColor;

in vec3 fragInColor;
in vec2 fragInTexCoord;

uniform sampler2D tex;

void main(void)
{
    FragColor = texture2D(tex, fragInTexCoord) * vec4(fragInColor, 1.0);
}
