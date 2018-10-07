#version 330 core
struct Light {
    vec3 position;
    vec3 direction;
    float innerCutOff; // cos(cutoff angle)
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

out vec4 fragColor;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

uniform vec3 viewPosition;
uniform Light light;
uniform Material material;

// Phong shading
void main(void)
{
    // Attenuation
    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant +
                               light.linear * distance +
                               light.quadratic * distance * distance);

    vec3 diffuseMapPoint = vec3(texture2D(material.diffuse, fragTexCoords));

    vec3 lightToFragmentDirection = normalize(fragPosition - light.position);

    // Angle of fragment from light's direction
    float theta = dot(lightToFragmentDirection, normalize(light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon,
                            0.0, 1.0);

    // Ambient
    vec3 ambient = light.ambient * diffuseMapPoint;

    // Diffuse
    float lightAngle = dot(fragNormal, -lightToFragmentDirection);
    vec3 diffuse = max(lightAngle, 0.0) *
            light.diffuse *
            diffuseMapPoint *
            intensity;

    // Specular
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectDirection = reflect(lightToFragmentDirection, fragNormal);
    float specularViewAngle = dot(viewDirection, reflectDirection);
    vec3 specular = pow(max(specularViewAngle, 0.0), material.shininess) *
            light.specular *
            vec3(texture2D(material.specular, fragTexCoords)) *
            intensity;

    fragColor = vec4((ambient + diffuse + specular) * attenuation,
                     1.0);
}
