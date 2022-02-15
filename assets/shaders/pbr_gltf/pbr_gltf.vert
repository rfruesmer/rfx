#version 450
#rfx

#include <punctual.glsl>

layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewMatrix;
    mat4 projMatrix;
    mat4 viewProjMatrix;
    vec3 cameraPosition;
    float pad;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    Light lights[MAX_LIGHTS];
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    float metallicFactor;
    float roughnessFactor;
    float pad0;
    float pad1;
} material;

layout(set = 3, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;


layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outPosition;

// ---------------------------------------------------------------------------------------------------------------------

vec4 getPosition()
{
    vec4 pos = vec4(inPosition, 1.0);

#ifdef USE_MORPHING
    pos += getTargetPosition(gl_VertexID);
#endif

#ifdef USE_SKINNING
    pos = getSkinningMatrix() * pos;
#endif

    return pos;
}

// ---------------------------------------------------------------------------------------------------------------------

#ifdef HAS_NORMAL_VEC3
vec3 getNormal()
{
    vec3 normal = inNormal;

#ifdef USE_MORPHING
    normal += getTargetNormal(gl_VertexID);
#endif

#ifdef USE_SKINNING
    normal = mat3(getSkinningNormalMatrix()) * normal;
#endif

    return normalize(normal);
}
#endif

// ---------------------------------------------------------------------------------------------------------------------

void main()
{
    vec4 pos = mesh.modelMatrix * getPosition();
    outPosition = vec3(pos.xyz) / pos.w;

#ifdef HAS_NORMAL_VEC3
#ifdef HAS_TANGENT_VEC4
    vec3 tangent = getTangent();
    vec3 normalW = normalize(vec3(u_NormalMatrix * vec4(getNormal(), 0.0)));
    vec3 tangentW = normalize(vec3(u_ModelMatrix * vec4(tangent, 0.0)));
    vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;
    v_TBN = mat3(tangentW, bitangentW, normalW);
#else
    mat3 normalMatrix = mat3(mesh.modelMatrix);
    outNormal = normalize(normalMatrix * inNormal);
#endif
#endif

#ifdef HAS_TEXCOORD_VEC2
    outTexCoord[0] = inTexCoord[0];
#endif

    gl_Position = scene.viewProjMatrix * pos;
}

// ---------------------------------------------------------------------------------------------------------------------
