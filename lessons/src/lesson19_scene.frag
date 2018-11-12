#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
struct Material {
    sampler2D ambientTexture0;
    sampler2D diffuseTexture0;
    sampler2D specularTexture0;
    float shininess;
};

out vec4 fragColor;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTextureCoordinates;

uniform Material material;
uniform vec3 camPosition;
uniform samplerCube skybox;

void main()
{
    vec3 viewDirection = normalize(fragPosition - camPosition);
    vec3 reflectDirection = reflect(viewDirection, normalize(fragNormal));
    vec4 reflectColor = vec4(textureCube(skybox, reflectDirection).rgb *
                             texture2D(material.ambientTexture0, fragTextureCoordinates).rgb,
                             1.0);

    fragColor = texture2D(material.diffuseTexture0, fragTextureCoordinates) + reflectColor;
}
