#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view_projection;
uniform mat3 normal;

void main() {
    gl_Position = view_projection * model * vec4(aPos, 1.0);
    fragNormal = normal * aNormal;
    fragPosition = vec3(model * vec4(aPos, 1.0));
    texCoords = aTexCoords;
}
