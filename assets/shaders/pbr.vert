#version 450

struct Light {
    vec3 position;
    float pad0;

    vec3 color;
    float pad1;

    bool enabled;
    float pad2;
    float pad3;
    float pad4;
};

layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewMatrix;
    mat4 projMatrix;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    Light lights[4];
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec3 baseColor;
    float pad0;

    float metallic;
    float roughness;
    float ao;
    float pad1;
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

    outPosition = (modelViewMatrix * vec4(inPosition, 1.0)).xyz;
    outNormal =  mat3(modelViewMatrix) * inNormal;

    gl_Position = scene.projMatrix * modelViewMatrix * vec4(inPosition, 1.0);
}
