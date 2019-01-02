#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinates;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;
uniform mat3 normal;

uniform bool reverseNormals;

out VS_OUT {
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 fragTextureCoordinates;
} vs_out;

void main(void)
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    vs_out.fragPosition = vec3(model * vec4(vertexPosition, 1.0));
    vs_out.fragNormal = normalize(normal * vertexNormal) *
            (reverseNormals ? -1.0 : 1.0);
    vs_out.fragTextureCoordinates = vertexTextureCoordinates;
}
