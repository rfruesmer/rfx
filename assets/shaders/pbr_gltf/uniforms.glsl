
layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewProjMatrix;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    mat4 pad0;
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec3 baseColor;
    float pad0;
} material;

layout(set = 3, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;
