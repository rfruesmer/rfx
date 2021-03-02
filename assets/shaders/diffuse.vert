#version 450

layout(set = 0, binding = 0)
uniform UniformBufferObject {
    mat4 viewMatrix;
    mat4 projMatrix;
    mat3 normalMatrix;
    vec4 lightPos;          // light position in eye coords
} ubo;

layout(set = 1, binding = 0)
uniform MeshUniformBufferObject {
    mat4 modelMatrix;
} meshUBO;


layout(push_constant)
uniform PushConstants {
    vec4 Ld;                // diffuse light intensity
    vec4 Kd;                // diffuse reflectivity (material property)
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outLightIntensity;


void main() {
    mat4 modelViewMatrix = ubo.viewMatrix * meshUBO.modelMatrix;
//    mat4 modelViewMatrix = ubo.viewMatrix;
    mat4 modelViewProjMatrix = ubo.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));

    vec3 normal = normalize(normalMatrix * inNormal);
    vec4 eyeCoords = modelViewMatrix * vec4(inPosition, 1.0);
    vec3 s = normalize(vec3(ubo.lightPos - eyeCoords));

    outLightIntensity = vec3(pc.Ld * pc.Kd * max(dot(s, normal), 0.0));

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);
}
