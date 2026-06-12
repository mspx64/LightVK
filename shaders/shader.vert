#version 460
#extension GL_EXT_descriptor_heap : require
#extension GL_EXT_nonuniform_qualifier : require

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 tangent;   
};

layout(push_constant) uniform PushData {
    uint frameIndex;
    uint vertexBufferIndex;
    uint indexBufferIndex;
    uint materialIndex;
}
pushData;

// Added instance names (e.g., 'vertexHeaps') to the descriptor heap arrays
layout(std430, descriptor_heap) buffer VertexBuffer {
    Vertex vertices[];
}
vertexHeaps[];

layout(std430, descriptor_heap) buffer IndexBuffer {
    uint indices[];
}
indexHeaps[];

layout(std140, descriptor_heap) uniform UBO {
    vec4 color; // Changed to vec4 to prevent CPU/GPU alignment mismatch headaches
}
uniformHeaps[];

layout(location = 0) out vec3 fragColor;

void main() {
    uint vertexIdx = indexHeaps[pushData.indexBufferIndex].indices[gl_VertexIndex];

    vec3 pos    = vertexHeaps[pushData.vertexBufferIndex].vertices[nonuniformEXT(vertexIdx)].position;
    gl_Position = vec4(pos, 2.0);
    fragColor   = uniformHeaps[pushData.frameIndex].color.rgb;
}