#version 450

layout(set = 0, binding = 0)
uniform UniformBufferObject {
    // Camera info
    mat4 viewMatrix;
    mat4 projMatrix;
    mat3 normalMatrix;

    // Light info
    vec4 lightPos;          // light position in eye coords
    vec3 La;                // Ambient light intensity
    vec3 Ld;                // Diffuse light intensity
    vec3 Ls;                // Specular light intensity
} ubo;

layout(set = 1, binding = 0)
uniform MeshUniformBufferObject {
    // Transformation
    mat4 modelMatrix;

    // Material info
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
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
    mat4 modelViewProjMatrix = ubo.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));

    vec3 normal = normalize(normalMatrix * inNormal);
    vec4 eyeCoords = modelViewMatrix * vec4(inPosition, 1.0);
    vec3 s = normalize(vec3(ubo.lightPos - eyeCoords));

    outLightIntensity = vec3(pc.Ld * pc.Kd * max(dot(s, normal), 0.0));

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);
}
