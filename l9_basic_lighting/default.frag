#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 fragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 camPosition;

void main() {
    // Ambient lighting
    float ambient = 0.1;

    // Diffuse lighting
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(fragPosition - lightPosition);
    float diffuse = max(dot(normal, -lightDirection), 0.0);

    // Specular lighting
    vec3 camDirection = normalize(camPosition - fragPosition);
    vec3 reflectDirection = reflect(lightDirection, normal);
    float specular = pow(max(dot(camDirection, reflectDirection), 0.0), 32);

    float lighting = ambient + diffuse + specular;
    fragColor = vec4(lighting * lightColor * objectColor, 1.0);
}
