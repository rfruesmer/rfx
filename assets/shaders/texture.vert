#version 450

layout(binding = 0)
uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float lodBias;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outFragTexCoord;
layout (location = 1) out float outLodBias;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    outFragTexCoord = inTexCoord;
    outLodBias = ubo.lodBias;
}
