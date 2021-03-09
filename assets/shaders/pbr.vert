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

    vec3 cameraPos;
    float pad0;

    Light lights[4];
} scene;

layout(set = 1, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec3 baseColor;
    float pad0;

    float metallic;
    float roughness;
    float ao;
    float pad1;
} material;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;

void main() {
    outPosition = (mesh.modelMatrix * vec4(inPosition, 1.0)).xyz;
    outNormal = mat3(mesh.modelMatrix) * inNormal;

    gl_Position = scene.projMatrix * scene.viewMatrix * vec4(outPosition, 1.0);
}
