#version 330 core
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
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
    vec3 diffuseMapPoint = vec3(texture2D(material.diffuse, fragTexCoords));

    // Ambient
    vec3 ambient = light.ambient * diffuseMapPoint;

    // Diffuse
    vec3 lightDirection = normalize(light.position - fragPosition);
    float lightAngle = dot(fragNormal, lightDirection);
    vec3 diffuse = max(lightAngle, 0.0) * light.diffuse * diffuseMapPoint;

    // Specular
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectDirection = reflect(-lightDirection, fragNormal);
    float specularViewAngle = dot(viewDirection, reflectDirection);
    vec3 specular = pow(max(specularViewAngle, 0.0), material.shininess) *
            light.specular *
            vec3(texture2D(material.specular, fragTexCoords));

    vec3 emission = vec3(texture2D(material.emission, fragTexCoords));

    fragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}
