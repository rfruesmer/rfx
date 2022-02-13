#version 450

layout(set = 0, binding = 0)
uniform UBO {
    mat4 viewProjMatrix;
    float blurFactor;
    int mipCount;
} ubo;


layout(location = 0)
in vec3 inPosition;

layout (location = 0)
out vec3 outUVW;


void main()
{
    outUVW = inPosition;

    gl_Position = ubo.viewProjMatrix * vec4(inPosition.xyz, 1.0);
}
