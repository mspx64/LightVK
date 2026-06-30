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
    mat4 transform;
}
pushData;

layout(std430, descriptor_heap) buffer VertexBuffer {
    Vertex vertices[];
}
vertexHeaps[];

layout(std430, descriptor_heap) buffer IndexBuffer {
    uint indices[];
}
indexHeaps[];

layout(std140, descriptor_heap) uniform UBO {
    mat4 view;
    mat4 proj;
    vec4 color;
}
uniformHeaps[];

layout(location = 0) out vec3 fragColor;

void main() {
    uint vertexIdx = indexHeaps[pushData.indexBufferIndex].indices[gl_VertexIndex];
    // mat4 view      = uniformHeaps[nonuniformEXT(pushData.frameIndex)].view;

    vec3 pos = vertexHeaps[pushData.vertexBufferIndex].vertices[nonuniformEXT(vertexIdx)].position;

    mat4 view = mat4(uniformHeaps[pushData.frameIndex].view[0],
                     uniformHeaps[pushData.frameIndex].view[1],
                     uniformHeaps[pushData.frameIndex].view[2],
                     uniformHeaps[pushData.frameIndex].view[3]);

    mat4 proj = mat4(uniformHeaps[pushData.frameIndex].proj[0],
                     uniformHeaps[pushData.frameIndex].proj[1],
                     uniformHeaps[pushData.frameIndex].proj[2],
                     uniformHeaps[pushData.frameIndex].proj[3]);

    gl_Position = proj * view * pushData.transform * vec4(pos, 1.0);

    fragColor = uniformHeaps[pushData.frameIndex].color.rgb;
}