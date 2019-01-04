#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinates;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBitangent;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;
uniform mat3 normal;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

out VS_OUT {
    vec3 fragPosition;
    vec2 fragTextureCoordinates;
    vec3 tangentLightPosition;
    vec3 tangentViewPosition;
    vec3 tangentFragPosition;
} vs_out;

void main(void)
{
    vec3 T = normalize(normal * vertexTangent);
    vec3 B = normalize(normal * vertexBitangent);
    vec3 N = normalize(normal * vertexNormal);
    mat3 invTBN = transpose(mat3(T, B, N));

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    vs_out.fragPosition = vec3(model * vec4(vertexPosition, 1.0));
    vs_out.fragTextureCoordinates = vertexTextureCoordinates;
    vs_out.tangentLightPosition = invTBN * lightPosition;
    vs_out.tangentViewPosition = invTBN * viewPosition;
    vs_out.tangentFragPosition = invTBN * vs_out.fragPosition;
}
