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

struct Lighting {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight {
    vec3 direction;
    Lighting lighting;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    Lighting lighting;
};

#define NUM_POINT_LIGHTS 4
uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform vec3 camPosition;

vec3 calculateBaseLight(vec3 lightDirection, Lighting lighting);
vec3 calculateDirectionalLight();
vec3 calculatePointLight();

void main() {
    vec3 lighting = calculateDirectionalLight();
    lighting += calculatePointLight();
    fragColor = vec4(lighting, 1.0);
}

vec3 calculateBaseLight(vec3 lightDirection, Lighting lighting) {
    // Diffuse lighting
    vec3 normal = normalize(fragNormal);
    float diffuse = max(dot(normal, -lightDirection), 0.0);

    // Specular lighting
    vec3 camDirection = normalize(camPosition - fragPosition);
    vec3 reflectDirection = reflect(lightDirection, normal);
    float specular = pow(max(dot(camDirection, reflectDirection), 0.0),
                         material.shine);

    vec3 diffuseColor = vec3(texture(material.diffuse, texCoords));
    vec3 specularColor = vec3(texture(material.specular, texCoords));

    return diffuseColor * lighting.ambient +
        diffuse * diffuseColor * lighting.diffuse +
        specular * specularColor * lighting.specular;
}

vec3 calculateDirectionalLight() {
    return calculateBaseLight(normalize(directionalLight.direction),
                              directionalLight.lighting);
}

vec3 calculatePointLight() {
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
        float distance = length(pointLights[i].position - fragPosition);
        float attenuation = 1.0 / (pointLights[i].constant +
                                   pointLights[i].linear * distance +
                                   pointLights[i].quadratic * distance * distance);
        vec3 lightDirection = normalize(fragPosition - pointLights[i].position);
        lighting += (calculateBaseLight(lightDirection, pointLights[i].lighting) *
                     attenuation);
    }
    return lighting;
}
