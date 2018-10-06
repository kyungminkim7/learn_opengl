#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

out vec3 fragPosition;
out vec3 fragNormal;

void main(void)
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);

    fragPosition = vec3(model * vec4(vertexPosition, 1.0));
    fragNormal = normalize(normal * vertexNormal);
}
