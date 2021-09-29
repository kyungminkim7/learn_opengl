#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view_projection;

void main() {
    gl_Position = view_projection * model * vec4(aPos, 1.0);
}
