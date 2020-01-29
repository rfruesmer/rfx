#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

struct Light {
    int type;
    vec3 position;
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
    
    float nDotL = max(0.0, dot(inNormal, ubo.light.position));

    outColor = vec4(0.0f);
    outColor += ubo.light.ambient * ubo.material.ambient;
    outColor += ubo.light.diffuse * ubo.material.diffuse * nDotL;

    if (nDotL > 0.0) {
        vec4 viewVector = normalize(ubo.mv * position);
        vec3 halfVector = normalize(ubo.light.position - vec3(viewVector));    
        float nDotH = max(0.0, dot(inNormal, halfVector));
        outColor += max(0.0, pow(nDotH, ubo.material.shininess))
            * ubo.material.specular
            * ubo.light.specular;
    }
}