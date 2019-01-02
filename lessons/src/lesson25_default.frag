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
    sampler2D diffuseTexture0;
    float shininess;
};

out vec4 fragColor;

in VS_OUT {
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 fragTextureCoordinates;
    vec4 fragPositionLightSpace;
} fs_in;

uniform vec3 viewPosition;
uniform Material material;

uniform PointLight light;

uniform sampler2D shadowMap;

Lighting calculateBaseLight(vec3 lightDirection, Lighting lighting);
float calculateShadow(vec3 lightDirection);

void main(void) {
    vec3 lightDirection = normalize(fs_in.fragPosition - light.position);
    Lighting result = calculateBaseLight(lightDirection, light.lighting);
    fragColor = vec4(result.ambient +
                     (1.0 - calculateShadow(lightDirection)) * (result.diffuse + result.specular),
                     1.0);
}

Lighting calculateBaseLight(vec3 lightDirection, Lighting lighting) {
    Lighting result;

    // Sets ambient color the same as the diffuse color
    vec3 materialDiffuse = texture2D(material.diffuseTexture0, fs_in.fragTextureCoordinates).rgb;
    result.ambient = lighting.ambient * materialDiffuse;

    // Fragment is brighter the closer it is aligned to the light ray direction
    float lightAngle = max(dot(fs_in.fragNormal, -lightDirection),
                           0.0);
    result.diffuse = lighting.diffuse * lightAngle * materialDiffuse;

    // Specular light is brighter the closer the angle btwn the reflected
    // light ray and the viewing vector.
    vec3 viewDirection = normalize(viewPosition - fs_in.fragPosition);
    vec3 halfwayDirection = normalize(-lightDirection + viewDirection);
    float specularAngle = dot(halfwayDirection, fs_in.fragNormal);

    result.specular = lighting.specular *
            pow(max(specularAngle, 0.0), material.shininess);

    return result;
}

float calculateShadow(vec3 lightDirection) {
    vec3 projCoords = fs_in.fragPositionLightSpace.xyz / fs_in.fragPositionLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = max(0.05 * (1.0 - dot(fs_in.fragNormal, -lightDirection)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture2D(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z <= 1.0 && projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}
