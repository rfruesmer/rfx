#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "light.glsl"
#include "material.glsl"
#include "directional_light.glsl"

layout (std140, binding = 0) uniform UniformBufferObject {
    mat4 mv;
    Light light;
    Material material;
} ubo;

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 0) out vec4 outColor;

void main() {
    vec4 ambient = vec4(0.0f);
    vec4 diffuse = vec4(0.0f);
    vec4 specular = vec4(0.0f);

    DirectionalLight(ubo.light, inNormal, ubo.material, ubo.mv, inPosition, ambient, diffuse, specular);    

    outColor = ambient * ubo.material.ambient
        + diffuse * ubo.material.diffuse
        + specular * ubo.material.specular;

    outColor = clamp(outColor, 0.0, 1.0);
}
