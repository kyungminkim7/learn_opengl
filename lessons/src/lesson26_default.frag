#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
struct Lighting {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    Lighting lighting;
    float farPlane;
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
} fs_in;

uniform vec3 viewPosition;
uniform Material material;

uniform PointLight light;

uniform samplerCube shadowMap;

#define NUM_SAMPLES 20
vec3 sampleOffsetDirections[NUM_SAMPLES] = vec3[](
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

Lighting calculateBaseLight(vec3 lightDirection, Lighting lighting);
float calculateShadow(vec3 lightRay);

void main(void) {
    vec3 lightRay = fs_in.fragPosition - light.position;
    Lighting result = calculateBaseLight(normalize(lightRay), light.lighting);
    fragColor = vec4(result.ambient +
                     (1.0 - calculateShadow(lightRay)) * (result.diffuse + result.specular),
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

float calculateShadow(vec3 lightRay) {
    // Increase offset radius by distance to viewer to make
    // closer shadows sharper.
    float viewDistance = length(viewPosition - fs_in.fragPosition);
    float diskRadius = (1.0 + (viewDistance / light.farPlane)) / 25.0;

    float bias = 0.15;
    float currentDepth = length(lightRay) - bias;

    float shadow = 0.0;
    for (int i = 0; i < NUM_SAMPLES; ++i) {
        float texelDepth = textureCube(shadowMap,
                                       lightRay + sampleOffsetDirections[i] * diskRadius).r;
        texelDepth *= light.farPlane;

        if (currentDepth > texelDepth) {
            shadow += 1.0;
        }
    }

    return shadow / NUM_SAMPLES;
}
