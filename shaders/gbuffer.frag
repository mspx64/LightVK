#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec4 outPosition; // G-Buffer 0
layout(location = 1) out vec4 outNormal;   // G-Buffer 1
layout(location = 2) out vec4 outAlbedo;   // G-Buffer 2

void main() {
    outPosition = vec4(inPosition, 1.0);
    outNormal   = vec4(normalize(inNormal), 1.0);
    outAlbedo   = vec4(inColor, 1.0);
}
