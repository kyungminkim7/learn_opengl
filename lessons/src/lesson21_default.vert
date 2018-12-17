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

out VS_OUT {
    vec3 normal;
} vs_out;

out vec2 fragTextureCoordinates;

void main(void)
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    fragTextureCoordinates = vertexTextureCoordinates;
    vs_out.normal = normalize(vec3(projection * vec4(normal * vertexNormal, 0)));
}
