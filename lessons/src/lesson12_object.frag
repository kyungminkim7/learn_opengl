#version 330 core
// Implements Phong shading

struct Lighting {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

// Light source infinitely far away with parallel light rays
// with no attenuation like the sun.
struct DirectionalLight {
    vec3 direction;

    Lighting lighting;
};

// Light source that illuminates in all directions but the
// light rays fade with distance like light bulbs/torches.
struct PointLight {
    vec3 position;

    Lighting lighting;

    Attenuation attenuation;
};

// Light source which shoots light rays in a specific direction.
// All objects within the inner cutoff are fully illuminated.
// Illumination btwn the inner and outer cutoffs fades out.
// All objects outside the outer cutoff exhibits ambient light.
struct SpotLight {
    vec3 position;
    vec3 direction;

    float innerCutOff; // cos(cutoff angle)
    float outerCutOff; // cos(cutoff angle)

    Lighting lighting;

    Attenuation attenuation;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

#define NUM_POINT_LIGHTS 4
out vec4 fragColor;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTextureCoordinates;

uniform vec3 viewPosition;
uniform Material material;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

Lighting calculateBaseLight(vec3 lightDirection, Lighting lighting);
float calculateAttenuation(vec3 lightPosition, Attenuation attenuation);
vec3 calculateDirectionalLight(DirectionalLight light);
vec3 calculatePointLight(PointLight light);
vec3 calculateSpotLight(SpotLight light);

void main(void) {
    vec3 color = calculateDirectionalLight(directionalLight);

    for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
        color += calculatePointLight(pointLights[i]);
    }

    color += calculateSpotLight(spotLight);

    fragColor = vec4(color, 1.0);
}

Lighting calculateBaseLight(vec3 lightDirection, Lighting lighting) {
    Lighting result;

    // Sets ambient color the same as the diffuse color
    vec3 materialDiffuse = texture2D(material.diffuse, fragTextureCoordinates).rgb;
    result.ambient = lighting.ambient * materialDiffuse;

    // Fragment is brighter the closer it is aligned to the light ray direction
    float lightAngle = max(dot(fragNormal, -lightDirection),
                           0.0);
    result.diffuse = lighting.diffuse * lightAngle * materialDiffuse;

    // Specular light is brighter the closer the angle btwn the reflected
    // light ray and the viewing vector.
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectDirection = reflect(lightDirection, fragNormal);
    float specularViewAngle = dot(viewDirection, reflectDirection);
    result.specular = lighting.specular *
            pow(max(specularViewAngle, 0.0), material.shininess) *
            texture2D(material.specular, fragTextureCoordinates).rgb;

    return result;
}

float calculateAttenuation(vec3 lightPosition, Attenuation attenuation) {
    float distance = length(lightPosition - fragPosition);
    return 1.0 / (attenuation.constant +
                  attenuation.linear * distance +
                  attenuation.quadratic * distance * distance);
}

vec3 calculateDirectionalLight(DirectionalLight light) {
    vec3 lightDirection = normalize(light.direction);
    Lighting result = calculateBaseLight(lightDirection, light.lighting);
    return result.ambient + result.diffuse + result.specular;
}

vec3 calculatePointLight(PointLight light) {
    vec3 lightDirection = normalize(fragPosition - light.position);
    Lighting result = calculateBaseLight(lightDirection, light.lighting);
    return (result.ambient + result.diffuse + result.specular) *
            calculateAttenuation(light.position, light.attenuation);
}

vec3 calculateSpotLight(SpotLight light) {
    vec3 lightDirection = normalize(fragPosition - light.position);

    // Calculate cone of brightness
    float theta = dot(lightDirection, normalize(light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon,
                            0.0, 1.0);

    Lighting result = calculateBaseLight(lightDirection, light.lighting);
    return (result.ambient + // Leave ambient to always have a little light
            (result.diffuse + result.specular) * intensity) *
            calculateAttenuation(light.position, light.attenuation);
}
