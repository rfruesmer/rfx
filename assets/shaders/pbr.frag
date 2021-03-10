#version 450

const float PI = 3.14159265358979323846;

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

    Light lights[4];
} scene;

layout(set = 1, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec3 baseColor;
    float pad0;

    float metallic;
    float roughness;
    float ao;
    float pad1;
} material;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outColor;

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

vec3 fresnelSchlick(float cosTheta)
{
    vec3 F0 = mix(vec3(0.04), material.baseColor, material.metallic);
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

// ---------------------------------------------------------------------------------------------------------------------

vec3 BRDF(int lightIndex, vec3 V, vec3 N)
{
    float nDotV = clamp(dot(N, V), 0.0, 1.0);
    if (nDotV <= 0.0) {
        return vec3(0.0);
    }

    vec3 L = scene.lights[lightIndex].position - V;
    float distance = length(L);
    L = normalize(L);

    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = scene.lights[lightIndex].color * attenuation;

    vec3 H = normalize(V + L);
    float nDotH = clamp(dot(N, H), 0.0, 1.0);
    float lDotH = clamp(dot(L, H), 0.0, 1.0);
    float nDotL = clamp(dot(N, L), 0.0, 1.0);

    float D = ggxDistribution(nDotH, material.roughness);
    float G = geometrySmith(nDotV, nDotL, material.roughness);
    vec3  F = fresnelSchlick(nDotV);

    vec3 diffuse = vec3(1.0) - F;
    diffuse *= (1.0 - material.metallic) * material.baseColor;

    vec3 numerator    = D * G * F;
    float denominator = 4.0 * nDotV * nDotL;
    vec3 specular     = numerator / max(denominator, 0.001);

    return (diffuse / PI + specular) * radiance * nDotL;
}

// ---------------------------------------------------------------------------------------------------------------------

void main() {
    vec3 N = normalize(inNormal);
    vec3 V = normalize(-inPosition);

    vec3 Lo = vec3(0);
    for(int i = 0; i < 4; ++i) {
        if (!scene.lights[i].enabled) {
            continue;
        }

        Lo += BRDF(i, V, N);
    }

    vec3 ambient = vec3(0.02) * material.baseColor * material.ao;
    vec3 color = ambient + Lo;

    outColor = color;
}


