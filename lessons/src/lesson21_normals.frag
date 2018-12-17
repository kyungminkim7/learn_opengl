#version 330 core
struct Material {
    sampler2D ambientTexture0;
    sampler2D diffuseTexture0;
    sampler2D specularTexture0;
    float shininess;
};

uniform Material material;

out vec4 fragColor;

in vec2 fragTextureCoordinates;

void main()
{
    fragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
