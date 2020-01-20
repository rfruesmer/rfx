#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

struct DirectionalLight {
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emissive;
    float shininess;
};

layout (std140, binding = 0) uniform UniformBufferObject {
    mat4 mvp;
    DirectionalLight light;
    Material material;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 0) out vec4 outColor;

void main() {
    gl_Position = ubo.mvp * vec4(inPosition, 1.0f);
    
    float nDotVP = max(0.0, dot(inNormal, ubo.light.direction));
    
    outColor = ubo.material.diffuse * ubo.light.diffuse * nDotVP;
}