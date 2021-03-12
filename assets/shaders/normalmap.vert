#version 450

#define MAX_LIGHTS 4

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

    vec3 camPos;
    float padding;

    // Light
    Light lights[MAX_LIGHTS];

    int useNormalMap;
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
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangentCamPos;
layout(location = 4) out vec3 outTangentPosition;
layout(location = 5) out vec3 outTangentLightPos[MAX_LIGHTS];


void main() {
    mat4 modelMatrix = mesh.modelMatrix;

    outPosition = (modelMatrix * vec4(inPosition, 1.0)).xyz;
    outTexCoord = inTexCoord;

    if (scene.useNormalMap != 0) {
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
    }
    else {
        mat3 normalMatrix = mat3(modelMatrix);
        outNormal = normalize(normalMatrix * inNormal);
    }

    gl_Position = scene.projMatrix * scene.viewMatrix * mesh.modelMatrix * vec4(inPosition, 1.0);
}
