#version 450

#define MAX_LIGHTS 4

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

    vec3 camPos;
    float padding;

    Light lights[MAX_LIGHTS];
} scene;

layout(set = 1, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColorFactor;
    vec4 emissiveFactor;
    float metallic;
    float roughness;
    uint baseColorTexCoordSet;
    uint metallicRoughnessTexCoordSet;
    uint normalTexCoordSet;
    uint emissiveTexCoordSet;
} material;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord[5];
layout(location = 7) in vec4 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexCoord[5];
layout(location = 6) out vec3 outTangentCamPos;
layout(location = 7) out vec3 outTangentPosition;
layout(location = 8) out vec3 outTangentLightPos[MAX_LIGHTS];


void main() {
    mat4 modelMatrix = mesh.modelMatrix;

    outPosition = (modelMatrix * vec4(inPosition, 1.0)).xyz;
    outTexCoord[0] = inTexCoord[0];
    outTexCoord[1] = inTexCoord[1];
    outTexCoord[2] = inTexCoord[2];
    outTexCoord[3] = inTexCoord[3];
    outTexCoord[4] = inTexCoord[4];

    mat3 normalMatrix = mat3(modelMatrix);
    vec3 T = normalize(normalMatrix * vec3(inTangent));
    vec3 N = normalize(normalMatrix * inNormal);

    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);

    vec3 B = normalize(cross(N, T)) * inTangent.w;
    mat3 TBN = transpose(mat3(T, B, N));

    outTangentCamPos   = TBN * scene.camPos;
    outTangentPosition = TBN * outPosition;
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        outTangentLightPos[i] = TBN * scene.lights[i].position;
    }

    gl_Position = scene.projMatrix * scene.viewMatrix * mesh.modelMatrix * vec4(inPosition, 1.0);
}
