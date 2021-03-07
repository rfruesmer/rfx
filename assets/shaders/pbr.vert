#version 450

struct Light {
    vec3 position;          // light position in eye coords
    float pad1;
    vec3 color;
    float pad2;
    vec3 direction;
    float pad3;
    float exponent;
    float cutoff;
    int type;
    bool enabled;
};

layout(set = 0, binding = 0)
uniform SceneData {
    // Camera
    mat4 viewMatrix;
    mat4 projMatrix;

    // Light
    vec4 ambientLight;
    Light lights[4];
} scene;

layout(set = 1, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    float shininess;
} material;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out mat3 outNormalMatrix;
layout(location = 6) out vec4 outTangent;

void main() {
    mat4 modelViewMatrix = scene.viewMatrix * mesh.modelMatrix;
    mat4 modelViewProjMatrix = scene.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));
    vec3 normal = normalize(normalMatrix * inNormal);

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);

    outPosition = (modelViewMatrix * vec4(inPosition, 1.0)).xyz;
    outTexCoord = inTexCoord;
    outNormal = normal;
    outNormalMatrix = normalMatrix;
    outTangent = vec4(mat3(mesh.modelMatrix) * inTangent.xyz, inTangent.w);
}
