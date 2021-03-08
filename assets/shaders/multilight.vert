#version 450

struct Light {
    int type;
    bool enabled;
    float pad1;
    float pad2;

    vec3 position;
    float pad3;

    vec3 color;
    float pad4;

    vec3 direction;
    float pad5;

    float spotCosInnerConeAngle;
    float spotCosOuterConeAngle;
    float range;
    float pad6;
};

layout(set = 0, binding = 0)
uniform SceneData {
    // Camera
    mat4 viewMatrix;
    mat4 projMatrix;

    // Light
    Light lights[4];
} scene;

layout(set = 1, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    vec3 specular;
    float shininess;
} material;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out mat3 outNormalMatrix;


void main() {
    mat4 modelViewMatrix = scene.viewMatrix * mesh.modelMatrix;
    mat4 modelViewProjMatrix = scene.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));
    vec3 normal = normalize(normalMatrix * inNormal);

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);

    outPosition = (modelViewMatrix * vec4(inPosition, 1.0)).xyz;
    outNormal = normal;
    outNormalMatrix = normalMatrix;
}
