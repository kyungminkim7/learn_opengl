#version 330 core
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out vec4 FragColor;

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 viewPosition;
uniform Light light;
uniform Material material;

// Phong shading
void main(void)
{
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 lightDirection = normalize(light.position - fragPosition);
    float lightAngle = dot(fragNormal, lightDirection);
    vec3 diffuse = max(lightAngle, 0.0) * light.diffuse * material.diffuse;

    // Specular
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectDirection = reflect(-lightDirection, fragNormal);
    float specularViewAngle = dot(viewDirection, reflectDirection);
    vec3 specular = pow(max(specularViewAngle, 0.0), material.shininess) *
            light.specular * material.specular;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
