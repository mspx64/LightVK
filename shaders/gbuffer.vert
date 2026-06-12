#version 450

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColor;

void main() {
    // Triangle vertices with hardcoded positions
    vec3 positions[3] = vec3[](
        vec3(0.0, -0.5, 0.0),
        vec3(0.5, 0.5, 0.0),
        vec3(-0.5, 0.5, 0.0)
    );
    
    vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0),  // Red
        vec3(0.0, 1.0, 0.0),  // Green
        vec3(0.0, 0.0, 1.0)   // Blue
    );
    
    vec3 pos = positions[gl_VertexIndex];
    outPosition = pos;
    outNormal = vec3(0.0, 0.0, 1.0);
    outColor = colors[gl_VertexIndex];
    
    gl_Position = vec4(pos, 1.0);
}
