#version 450

layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewMatrix;
    mat4 projMatrix;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    vec3 lightPos;
    float pad;
    vec3 lightColor;
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    vec3 specularFactor;
    float shininess;
} material;

layout(set = 3, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;


void main() {
    mat4 modelViewMatrix = scene.viewMatrix * mesh.modelMatrix;
    mat4 modelViewProjMatrix = scene.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));
    vec3 normal = normalize(normalMatrix * inNormal);

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);

    outPosition = (modelViewMatrix * vec4(inPosition, 1.0)).xyz;
    outNormal = normal;
}
