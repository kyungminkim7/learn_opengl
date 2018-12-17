#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float time;

in VS_OUT {
    vec2 textureCoordinates;
} gs_in[];

out GS_OUT {
    vec2 textureCoordinates;
} gs_out;

vec3 getNormal() {
    vec3 x = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 y = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(y, x));
}

vec4 explode(vec4 position, vec3 normalDirection) {
    vec3 explosionVector = normalDirection * (sin(time) + 1.0);
    return position + vec4(explosionVector, 0.0);
}

void main() {
    vec3 normalDirection = getNormal();

    gl_Position = explode(gl_in[0].gl_Position, normalDirection);
    gs_out.textureCoordinates = gs_in[0].textureCoordinates;
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normalDirection);
    gs_out.textureCoordinates = gs_in[1].textureCoordinates;
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normalDirection);
    gs_out.textureCoordinates = gs_in[2].textureCoordinates;
    EmitVertex();

    EndPrimitive();
}
