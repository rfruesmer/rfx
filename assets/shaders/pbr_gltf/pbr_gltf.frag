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


//layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

// ---------------------------------------------------------------------------------------------------------------------

vec4 getVertexColor()
{
    vec4 color = vec4(1.0);

#ifdef HAS_COLOR_0_VEC3
    color.rgb = v_Color.rgb;
#endif
#ifdef HAS_COLOR_0_VEC4
    color = v_Color;
#endif

    return color;
}

// ---------------------------------------------------------------------------------------------------------------------

vec4 getBaseColor()
{
    vec4 baseColor = vec4(1);

#if defined(MATERIAL_SPECULARGLOSSINESS)
    baseColor = u_DiffuseFactor;
#elif defined(MATERIAL_METALLICROUGHNESS)
    baseColor = material.baseColor;
#endif

#if defined(MATERIAL_SPECULARGLOSSINESS) && defined(HAS_DIFFUSE_MAP)
    baseColor *= texture(u_DiffuseSampler, getDiffuseUV());
#elif defined(MATERIAL_METALLICROUGHNESS) && defined(HAS_BASE_COLOR_MAP)
    baseColor *= texture(u_BaseColorSampler, getBaseColorUV());
#endif

    return baseColor * getVertexColor();
}

// ---------------------------------------------------------------------------------------------------------------------

void main() {
    vec4 baseColor = getBaseColor();

    outColor = baseColor;
}

// ---------------------------------------------------------------------------------------------------------------------
