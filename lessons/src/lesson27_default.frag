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

Lighting calculateBaseLight(Lighting lighting);

void main(void) {
    Lighting result = calculateBaseLight(light.lighting);
    fragColor = vec4(result.ambient + result.diffuse + result.specular,
                     1.0);
}

Lighting calculateBaseLight(Lighting lighting) {
    vec3 lightDirection = normalize(fs_in.tangentFragPosition - fs_in.tangentLightPosition);

    vec3 normal = texture2D(material.normalMap, fs_in.fragTextureCoordinates).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    Lighting result;

    // Sets ambient color the same as the diffuse color
    vec3 materialDiffuse = texture2D(material.diffuseTexture0, fs_in.fragTextureCoordinates).rgb;
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
