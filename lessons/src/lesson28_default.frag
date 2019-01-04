#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
struct Lighting {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    Lighting lighting;
};

struct Material {
    sampler2D diffuseTexture0;
    sampler2D normalMap;
    sampler2D depthMap;
    float heightScale;
    float shininess;
};

out vec4 fragColor;

in VS_OUT {
    vec3 fragPosition;
    vec2 fragTextureCoordinates;
    vec3 tangentLightPosition;
    vec3 tangentViewPosition;
    vec3 tangentFragPosition;
} fs_in;

uniform Material material;

uniform PointLight light;

vec2 calculateParallaxMapping();
Lighting calculateBaseLight(vec2 textureCoordinates, Lighting lighting);

void main(void) {
    vec2 displacedTextureCoordinates = calculateParallaxMapping();
    if (displacedTextureCoordinates.x > 1.0 || displacedTextureCoordinates.y > 1.0 ||
            displacedTextureCoordinates.x < 0.0 || displacedTextureCoordinates.y < 0.0) {
        discard;
    }

    Lighting result = calculateBaseLight(displacedTextureCoordinates, light.lighting);
    fragColor = vec4(result.ambient + result.diffuse + result.specular,
                     1.0);
}

vec2 calculateParallaxMapping() {
    vec3 viewDirection = normalize(fs_in.tangentViewPosition - fs_in.tangentFragPosition);
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirection)));
    float layerDepth = 1.0 / numLayers;

    vec2 p = viewDirection.xy * material.heightScale;
    vec2 deltaTexCoords = p / numLayers;

    float currentLayerDepth = 0.0;
    vec2 currentTexCoords = fs_in.fragTextureCoordinates;
    float currentDepthMapValue = texture2D(material.depthMap, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture2D(material.depthMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture2D(material.depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);
    return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
}

Lighting calculateBaseLight(vec2 textureCoordinates, Lighting lighting) {
    vec3 lightDirection = normalize(fs_in.tangentFragPosition - fs_in.tangentLightPosition);

    vec3 normal = texture2D(material.normalMap, textureCoordinates).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    Lighting result;

    // Sets ambient color the same as the diffuse color
    vec3 materialDiffuse = texture2D(material.diffuseTexture0, textureCoordinates).rgb;
    result.ambient = lighting.ambient * materialDiffuse;

    // Fragment is brighter the closer it is aligned to the light ray direction
    float lightAngle = max(dot(normal, -lightDirection),
                           0.0);
    result.diffuse = lighting.diffuse * lightAngle * materialDiffuse;

    // Specular light is brighter the closer the angle btwn the reflected
    // light ray and the viewing vector.
    vec3 viewDirection = normalize(fs_in.tangentViewPosition - fs_in.tangentFragPosition);
    vec3 halfwayDirection = normalize(-lightDirection + viewDirection);
    float specularAngle = dot(halfwayDirection, normal);

    result.specular = lighting.specular *
            pow(max(specularAngle, 0.0), material.shininess);

    return result;
}
