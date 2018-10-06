#version 330 core
out vec4 FragColor;

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 viewPosition;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;

// Phong shading
void main(void)
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDirection = normalize(lightPosition - fragPosition);
    float lightAngle = dot(fragNormal, lightDirection);
    vec3 diffuse = max(lightAngle, 0.0) * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectDirection = reflect(-lightDirection, fragNormal);
    float specularViewAngle = dot(viewDirection, reflectDirection);
    vec3 specular = specularStrength *
            pow(max(specularViewAngle, 0.0), 64) *
            lightColor;

    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
