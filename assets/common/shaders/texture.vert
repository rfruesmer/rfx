#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 0) out vec2 outTexCoord;

void main() {
    gl_Position = ubo.mvp * inPosition;
    outTexCoord = inTexCoord;
}