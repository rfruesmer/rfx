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
    vec3 cameraPosition;
    float pad;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    Light lights[MAX_LIGHTS];
} shader;

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

layout(set = 3, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 1)
uniform sampler2D baseColorTexture;

layout(set = 2, binding = 2)
uniform sampler2D normalTexture;

layout(set = 2, binding = 3)
uniform sampler2D metallicRoughnessTexture;

layout(set = 2, binding = 4)
uniform sampler2D occlusionTexture;

layout(set = 2, binding = 5)
uniform sampler2D emissiveTexture;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outColor;


const float PI = 3.14159265358979323846;

// ---------------------------------------------------------------------------------------------------------------------

float ggxDistribution(float nDotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = (nDotH * nDotH * (alpha2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return alpha2 / denom;
}

// ---------------------------------------------------------------------------------------------------------------------

float geometrySmith(float nDotV, float nDotL, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float ggxL  = nDotL / (nDotL * (1.0 - k) + k);
    float ggxV  = nDotV / (nDotV * (1.0 - k) + k);

    return ggxL * ggxV;
}

// ---------------------------------------------------------------------------------------------------------------------

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

// ---------------------------------------------------------------------------------------------------------------------

vec3 BRDF(
    int lightIndex,
    vec3 V,
    vec3 N,
    vec3 F0,
    vec3 baseColor,
    float metallic,
    float roughness)
{
    float nDotV = clamp(dot(N, V), 0.0, 1.0);
    if (nDotV <= 0.0) {
        return vec3(0.0);
    }

#ifdef HAS_NORMAL_MAP
    vec3 L = inTangentLightPos[lightIndex] - inTangentPosition;
#else
    vec3 L = shader.lights[lightIndex].position - inPosition;
#endif

    float distance = length(L);
    L = normalize(L);

    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = shader.lights[lightIndex].color * attenuation;

    vec3 H = normalize(V + L);
    float nDotH = clamp(dot(N, H), 0.0, 1.0);
    float lDotH = clamp(dot(L, H), 0.0, 1.0);
    float nDotL = clamp(dot(N, L), 0.0, 1.0);

    float D = ggxDistribution(nDotH, roughness);
    float G = geometrySmith(nDotV, nDotL, roughness);
    vec3  F = fresnelSchlick(nDotV, F0);

    vec3 diffuse = vec3(1.0) - F;
    diffuse *= (1.0 - metallic) * baseColor;

    vec3 numerator    = D * G * F;
    float denominator = 4.0 * nDotV * nDotL;
    vec3 specular     = (numerator / max(denominator, 0.001)) * baseColor;

    return (diffuse / PI + specular) * radiance * nDotL;
}

// ---------------------------------------------------------------------------------------------------------------------

vec4 sRGBtoLinear(vec4 srgb)
{
    vec3 linear = pow(srgb.rgb, vec3(2.2));
    return vec4(linear, srgb.w);
}

// ---------------------------------------------------------------------------------------------------------------------

void main()
{
    vec4 baseColor = material.baseColorFactor;

#ifdef HAS_BASE_COLOR_MAP
//    baseColor *= sRGBtoLinear(texture(baseColorTexture, inTexCoord[material.baseColorTexCoordSet]));
    baseColor *= texture(baseColorTexture, inTexCoord[material.baseColorTexCoordSet]);
#endif

    float metallic = material.metallic;
    float roughness = material.roughness;

#ifdef HAS_METALLIC_ROUGHNESS_MAP
    vec4 mr = texture(metallicRoughnessTexture, inTexCoord[material.metallicRoughnessTexCoordSet]);
    metallic *= mr.b;
    roughness *= mr.g;
#endif

#ifdef HAS_NORMAL_MAP
    vec3 N = texture(normalTexture, inTexCoord[material.normalTexCoordSet]).xyz * 2.0 - 1.0;
    vec3 V = normalize(inTangentCamPos - inTangentPosition);
#elif HAS_NORMALS
    vec3 N = inNormal;
    vec3 V = normalize(scene.camPos - inPosition);
#endif
    N = normalize(N);

    vec3 F0 = mix(vec3(0.04), baseColor.rgb, metallic);
    vec3 Lo = vec3(0);

    for(int i = 0; i < MAX_LIGHTS; ++i) {
        if (!shader.lights[i].enabled) {
            continue;
        }

        Lo += BRDF(i, V, N, F0, baseColor.rgb, metallic, roughness);
    }

    vec3 color = Lo;

#ifdef HAS_OCCLUSION_MAP
    float ao = texture(occlusionTexture, inTexCoord[material.occlusionTexCoordSet]).r;
    color = mix(color, color * ao, material.occlusionStrength);
#endif

#ifdef HAS_EMISSIVE_MAP
    vec3 emissive = sRGBtoLinear(texture(emissiveTexture, inTexCoord[material.emissiveTexCoordSet])).rgb
        * material.emissiveFactor.rgb
        * vec3(255.0); // TODO: check linear/sRGB

    color += emissive;
#endif

    // HDR tonemapping
//    color = color / (color + vec3(1.0));

    // gamma correct
//    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, baseColor.a);
}


