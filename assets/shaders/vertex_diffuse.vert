#version 450

layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewMatrix;
    mat4 projMatrix;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    vec4 lightPos;          // light position in eye coords
    vec4 La;                // Ambient light intensity
    vec4 Ld;                // Diffuse light intensity
    vec4 Ls;                // Specular light intensity
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
} material;

layout(set = 3, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outLightIntensity;


void main() {
    mat4 modelViewMatrix = scene.viewMatrix * mesh.modelMatrix;
    mat4 modelViewProjMatrix = scene.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));

    vec3 normal = normalize(normalMatrix * inNormal);
    vec4 eyeCoords = modelViewMatrix * vec4(inPosition, 1.0);
    vec3 s = normalize(vec3(shader.lightPos - eyeCoords));

    float sDotN = max(dot(s, normal), 0.0);
    vec3 diffuse = vec3(shader.Ld * material.baseColor) * sDotN;

    outLightIntensity = diffuse;

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);
}
