#version 450

layout(location = 0) in vec2 inUV;

layout(set = 0, binding = 0) uniform sampler2D positionTex;
layout(set = 0, binding = 1) uniform sampler2D normalTex;
layout(set = 0, binding = 2) uniform sampler2D albedoTex;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 position = texture(positionTex, inUV).rgb;
    vec3 normal = normalize(texture(normalTex, inUV).rgb);
    vec3 albedo = texture(albedoTex, inUV).rgb;
    
    // Simple directional lighting
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    vec3 lightColor = vec3(1.0);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Add ambient
    vec3 ambient = vec3(0.2);
    
    vec3 finalColor = (ambient + diffuse) * albedo;
    
    outColor = vec4(finalColor, 1.0);
}
