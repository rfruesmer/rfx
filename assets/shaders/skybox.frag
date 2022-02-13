#version 450

layout(set = 0, binding = 0)
uniform UBO {
    mat4 viewProjMatrix;
    float blurFactor;
    int mipCount;
} ubo;

layout (set = 0, binding = 1)
uniform samplerCube samplerCubeMap;

layout (location = 0)
in vec3 inUVW;

layout (location = 0)
out vec4 outFragColor;

void main()
{
    outFragColor = textureLod(samplerCubeMap, inUVW, ubo.blurFactor * float(ubo.mipCount -1));
}
