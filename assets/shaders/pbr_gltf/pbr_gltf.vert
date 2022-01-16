#version 450
#rfx

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
    mat4 pad0;
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
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

void main() {
    vec4 pos = mesh.modelMatrix * getPosition();
    outPosition = vec3(pos.xyz) / pos.w;

    gl_Position = scene.viewProjMatrix * pos;
}

// ---------------------------------------------------------------------------------------------------------------------
