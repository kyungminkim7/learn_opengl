#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4 shadowTransforms[6];

out vec4 fragPosition;

void main() {
    // Project each triangle onto a face of the cubemap
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face;

        for (int i = 0; i < 3; ++i) {
            fragPosition = gl_in[i].gl_Position;
            gl_Position = shadowTransforms[face] * fragPosition;
            EmitVertex();
        }

        EndPrimitive();
    }
}
