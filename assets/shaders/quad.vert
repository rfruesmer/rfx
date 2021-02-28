#version 450

layout(binding = 0)
uniform UniformBufferObject {
    mat4 proj;
    mat4 view;
} ubo;

layout(push_constant)
uniform PushConstants {
    mat4 model;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec4 outFragColor;


void main() {
    gl_Position = ubo.proj * ubo.view * pushConstants.model * vec4(inPosition, 1.0);
    outFragColor = vec4(inColor, 1.0);
}
