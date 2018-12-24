#version 330 core
struct Lighting {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    Lighting lighting;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

out vec4 fragColor;

in VS_OUT {
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 fragTextureCoordinates;
} fs_in;

uniform vec3 viewPosition;
uniform Material material;
uniform bool blinn;

uniform PointLight light;

Lighting calculateBaseLight(vec3 lightDirection, Lighting lighting);

void main(void) {
    vec3 lightDirection = normalize(fs_in.fragPosition - light.position);
    Lighting result = calculateBaseLight(lightDirection, light.lighting);
    fragColor = vec4(result.ambient + result.diffuse + result.specular, 1.0);
}

Lighting calculateBaseLight(vec3 lightDirection, Lighting lighting) {
    Lighting result;

    // Sets ambient color the same as the diffuse color
    vec3 materialDiffuse = texture2D(material.diffuse, fs_in.fragTextureCoordinates).rgb;
    result.ambient = lighting.ambient * materialDiffuse;

    // Fragment is brighter the closer it is aligned to the light ray direction
    float lightAngle = max(dot(fs_in.fragNormal, -lightDirection),
                           0.0);
    result.diffuse = lighting.diffuse * lightAngle * materialDiffuse;

    // Specular light is brighter the closer the angle btwn the reflected
    // light ray and the viewing vector.
    vec3 viewDirection = normalize(viewPosition - fs_in.fragPosition);
    vec3 reflectDirection = reflect(lightDirection, fs_in.fragNormal);

    float specularAngle = 0.0;
    if (blinn) {
        vec3 halfwayDirection = normalize(-lightDirection + viewDirection);
        specularAngle = dot(halfwayDirection, fs_in.fragNormal);
    } else {
        specularAngle = dot(viewDirection, reflectDirection);
    }

    result.specular = lighting.specular *
            pow(max(specularAngle, 0.0), material.shininess);

    return result;
}
