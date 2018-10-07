#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;

void main(void)
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);

    fragPosition = vec3(model * vec4(vertexPosition, 1.0));
    fragNormal = normalize(normal * vertexNormal);
    fragTexCoords = vertexTexCoords;
}
