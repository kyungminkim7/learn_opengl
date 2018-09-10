#version 330 core
out vec4 FragColor;

in vec3 fragInColor;
in vec2 fragInTexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main(void)
{
    FragColor = mix(texture2D(texture0, fragInTexCoord),
                    texture2D(texture1, fragInTexCoord),
                    0.2);
}
