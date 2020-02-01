#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "light.glsl"
#include "material.glsl"
#include "point_light.glsl"


layout (std140, binding = 0) uniform UniformBufferObject {
    mat4 mv;
    mat4 mvp;
    Light light;
    Material material;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 0) out vec4 outColor;

void main() {
    vec4 position = vec4(inPosition, 1.0f);

    gl_Position = ubo.mvp * position;
   
    vec4 ambient = vec4(0.0f);
    vec4 diffuse = vec4(0.0f);
    vec4 specular = vec4(0.0f);

    PointLight(ubo.light, inNormal, ubo.material, ubo.mv, position, ambient, diffuse, specular);    

    outColor = ambient * ubo.material.ambient
        + diffuse * ubo.material.diffuse
        + specular * ubo.material.specular;

    outColor = clamp(outColor, 0.0, 1.0);
}