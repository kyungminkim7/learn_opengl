#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 texCoords;

out vec4 fragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shine;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 camPosition;

void main() {
    // Diffuse lighting
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(fragPosition - light.position);
    float diffuse = max(dot(normal, -lightDirection), 0.0);

    // Specular lighting
    vec3 camDirection = normalize(camPosition - fragPosition);
    vec3 reflectDirection = reflect(lightDirection, normal);
    float specular = pow(max(dot(camDirection, reflectDirection), 0.0),
                         material.shine);

    vec3 diffuseColor = vec3(texture(material.diffuse, texCoords));
    vec3 specularColor = vec3(texture(material.specular, texCoords));

    vec3 lighting = diffuseColor * light.ambient +
            diffuse * diffuseColor * light.diffuse +
            specular * specularColor * light.specular;
    fragColor = vec4(lighting, 1.0);
}
