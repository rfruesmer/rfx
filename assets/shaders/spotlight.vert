#version 450

layout(set = 0, binding = 0)
uniform SceneData {
    // Camera
    mat4 viewMatrix;
    mat4 projMatrix;

    // Light
    vec3 lightPos;              // light position in eye coords
    float pad1;
    vec3 lightColor;
    float pad2;
    vec3 spotDirection;         // Direction of the spotlight in eye coords
    float pad3;
    float innerConeAngle;
    float outerConeAngle;
} scene;

layout(set = 1, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    vec3 specularFactor;
    float shininess;
} material;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outSpotDirection;

void main() {
    mat4 modelViewMatrix = scene.viewMatrix * mesh.modelMatrix;
    mat4 modelViewProjMatrix = scene.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));
    vec3 normal = normalize(normalMatrix * inNormal);

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);

    outPosition = (modelViewMatrix * vec4(inPosition, 1.0)).xyz;
    outNormal = normal;
    outSpotDirection = normalMatrix * scene.spotDirection;
}
