#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout (std140, binding = 0) uniform UniformBufferObject {
    mat4 mv;
    mat4 mvp;
} ubo;


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec3 outNormal;

void main() {

    vec4 position = vec4(inPosition, 1.0f);

    gl_Position = ubo.mvp * position;
   
    outPosition = ubo.mv * position;
    outNormal = mat3(ubo.mv) * inNormal;
}