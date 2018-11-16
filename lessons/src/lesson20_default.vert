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

out VF {
    vec3 position;
    vec3 normal;
    vec2 textureCoordinates;
} vf;

void main(void)
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    vf.position = vec3(model * vec4(vertexPosition, 1.0));
    vf.normal = normal * vertexNormal;
    vf.textureCoordinates = vertexTextureCoordinates;
}
