#version 450

layout(set = 0, binding = 0)
uniform Scene {
    mat4 viewMatrix;
    mat4 projMatrix;

    // Light
    vec4 lightPos;          // light position in eye coords
    vec4 Ld;                // diffuse light intensity
} scene;

layout(set = 1, binding = 0)
uniform Mesh {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform Material {
    vec4 Kd;                // diffuse reflectivity (material property)
} material;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outLightIntensity;


void main() {
    mat4 modelViewMatrix = scene.viewMatrix * mesh.modelMatrix;
    mat4 modelViewProjMatrix = scene.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));

    vec3 normal = normalize(normalMatrix * inNormal);
    vec4 eyeCoords = modelViewMatrix * vec4(inPosition, 1.0);
    vec3 s = normalize(vec3(scene.lightPos - eyeCoords));

    outLightIntensity = vec3(scene.Ld * material.Kd * max(dot(s, normal), 0.0));

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);
}
