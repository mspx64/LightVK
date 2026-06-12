#version 450

layout(location = 0) out vec2 outUV;

void main() {
    // Full screen quad
    vec2 positions[6] = vec2[](
        vec2(-1.0, -1.0),
        vec2(1.0, -1.0),
        vec2(1.0, 1.0),
        vec2(-1.0, -1.0),
        vec2(1.0, 1.0),
        vec2(-1.0, 1.0)
    );
    
    outUV = (positions[gl_VertexIndex] + 1.0) * 0.5;
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
