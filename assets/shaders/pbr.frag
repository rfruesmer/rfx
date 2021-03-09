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

    vec3 cameraPos;
    float pad0;

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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main()
{
    vec3 N = normalize(inNormal);
    vec3 V = normalize(scene.cameraPos - inPosition);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.baseColor, material.metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i)
    {
        if (!scene.lights[i].enabled) {
            continue;
        }

        // calculate per-light radiance
        vec3 L = normalize(scene.lights[i].position - inPosition);
        vec3 H = normalize(V + L);
        float distance    = length(scene.lights[i].position - inPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = scene.lights[i].color * attenuation;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, material.roughness);
        float G   = GeometrySmith(N, V, L, material.roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.metallic;

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * material.baseColor / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.01) * material.baseColor * material.ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    outColor = color;
}
