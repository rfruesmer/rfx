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

vec3 fresnelSchlick(float cosTheta)
{
    vec3 F0 = mix(vec3(0.04), material.baseColor, material.metallic);
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

// ---------------------------------------------------------------------------------------------------------------------

float DistributionGGX(float nDotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = (nDotH * nDotH * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

// ---------------------------------------------------------------------------------------------------------------------

float GeometrySchlickGGX(float nDotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = nDotV;
    float denom = nDotV * (1.0 - k) + k;

    return num / denom;
}

// ---------------------------------------------------------------------------------------------------------------------

float GeometrySmith(float nDotV, float nDotL, float roughness)
{
    float ggx2  = GeometrySchlickGGX(nDotV, roughness);
    float ggx1  = GeometrySchlickGGX(nDotL, roughness);

    return ggx1 * ggx2;
}

// ---------------------------------------------------------------------------------------------------------------------

vec3 BRDF(int lightIndex, vec3 V, vec3 N)
{
    vec3 L = scene.lights[lightIndex].position - V;
    float distance = length(L);
    L = normalize(L);

    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = scene.lights[lightIndex].color * attenuation;


    vec3 H = normalize(V + L);
    float nDotH = clamp(dot(N, H), 0.0, 1.0);
    float lDotH = clamp(dot(L, H), 0.0, 1.0);
    float nDotL = clamp(dot(N, L), 0.0, 1.0);
    float nDotV = clamp(dot(N, V), 0.0, 1.0);
//    float hDotV = clamp(dot(H, V), 0.0, 1.0);

    float D = DistributionGGX(nDotH, material.roughness);
    float G = GeometrySmith(nDotV, nDotL, material.roughness);
    vec3  F = fresnelSchlick(nDotV);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic;

    vec3 numerator    = D * G * F;
    float denominator = 4.0 * max(nDotV, 0.0) * nDotL;
    vec3 specular     = numerator / max(denominator, 0.001);

    return (kD * material.baseColor / PI + specular) * radiance * nDotL;
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
//    color = color / (color + vec3(1.0));
//    color = pow(color, vec3(0.4545)); // Gamma

    outColor = color;
}


