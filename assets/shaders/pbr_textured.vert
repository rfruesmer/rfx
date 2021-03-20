#version 450
#rfx


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
    int baseColorTexCoordSet;
    int metallicRoughnessTexCoordSet;
    int normalTexCoordSet;
    int occlusionTexCoordSet;
    float occlusionStrength;
    int emissiveTexCoordSet;
} material;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outPosition;


void main() {
    mat4 modelMatrix = mesh.modelMatrix;
    mat3 normalMatrix = mat3(modelMatrix);

    outPosition = (modelMatrix * vec4(inPosition, 1.0)).xyz;

#ifdef HAS_NORMALS
    outNormal = normalize(normalMatrix * inNormal);
#endif

#ifdef HAS_NORMAL_MAP
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
#endif

#ifdef TEXCOORDSET_COUNT
    for (int i = 0; i < TEXCOORDSET_COUNT; ++i) {
        outTexCoord[i] = inTexCoord[i];
    }
#endif

    gl_Position = scene.projMatrix * scene.viewMatrix * mesh.modelMatrix * vec4(inPosition, 1.0);
}
