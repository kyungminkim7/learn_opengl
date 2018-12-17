#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

const float MAGNITUDE = 0.1;

in VS_OUT {
    vec3 normal;
} gs_in[];

void generateNormalLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();

    EndPrimitive();
}

void main() {
    for (int i = 0; i < 2; ++i) {
        generateNormalLine(i);
    }
}
