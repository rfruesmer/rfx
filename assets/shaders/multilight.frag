#version 450

struct Light {
    int type;
    bool enabled;
    float pad1;
    float pad2;

    vec3 position;
    float pad3;

    vec3 color;
    float pad4;

    vec3 direction;
    float pad5;

    float spotCosInnerConeAngle;
    float spotCosOuterConeAngle;
    float range;
    float pad6;
};

layout(set = 0, binding = 0)
uniform SceneData {
    // Camera
    mat4 viewMatrix;
    mat4 projMatrix;

    // Light
    Light lights[4];
} scene;

layout(set = 1, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    vec3 specular;
    float shininess;
} material;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in mat3 inNormalMatrix;

layout(location = 0) out vec3 outColor;


vec3 pointLight(int index, vec3 position, vec3 normal)
{
    vec3 lightDirection = scene.lights[index].position - position;
    float lightDistance = length(lightDirection);
    lightDirection /= lightDistance;

    float attenuation = 1.0;
    if (scene.lights[index].range > 0.0) {
        attenuation = max(min(1.0f - (lightDistance / scene.lights[index].range), 1), 0) / lightDistance;
    }

    float nDotL = max(0.0, dot(normal, lightDirection));
    vec3 diffuse =  material.baseColor.xyz * nDotL;

    vec3 specular = vec3(0.0);
    if (nDotL > 0.0 && material.shininess > 0.0) {
        vec3 eyeDirection = normalize(-position);
        vec3 halfVector = normalize(eyeDirection + lightDirection);
        float nDotH = max(0.0, dot(normal, halfVector));
        specular = material.specular * pow(nDotH, material.shininess);
    }

    return scene.lights[index].color * (diffuse + specular) * attenuation;
}

vec3 spotLight(int index, vec3 position, vec3 normal)
{
    vec3 lightDirection = normalize(scene.lights[index].position - position);
    vec3 spotDirection = normalize(inNormalMatrix * scene.lights[index].direction);
    float spotCos = dot(lightDirection, -spotDirection);
    if (spotCos <= scene.lights[index].spotCosOuterConeAngle) {
        return vec3(0.0);
    }

    float attenuation = 0.0;
#if 0
    float lightAngleScale = 1.0f /
        max(0.001f, scene.lights[index].spotCosInnerConeAngle - scene.lights[index].spotCosOuterConeAngle);
    float lightAngleOffset = -scene.lights[index].spotCosOuterConeAngle * lightAngleScale;
    attenuation = clamp(spotCos * lightAngleScale + lightAngleOffset, 0.0, 1.0);
    attenuation *= attenuation;
#else
    float epsilon = scene.lights[index].spotCosInnerConeAngle - scene.lights[index].spotCosOuterConeAngle;
    attenuation = clamp((spotCos - scene.lights[index].spotCosOuterConeAngle) / epsilon, 0.0, 1.0);
#endif

    float nDotL = max(0.0, dot(normal, lightDirection));
    vec3 diffuse = material.baseColor.xyz * nDotL;

    vec3 specular = vec3(0);
    if (nDotL > 0.0 && material.shininess > 0.0) {
        vec3 eyeDirection = normalize(-position);
        vec3 halfVector = normalize(eyeDirection + lightDirection);
        float nDotH = max(0.0, dot(normal, halfVector));
        specular = material.specular * pow(nDotH, material.shininess);
    }

    return scene.lights[index].color * (diffuse + specular) * attenuation;
}

void main()
{
    vec3 color = vec3(0.0);
    vec3 normal = normalize(inNormal);

    for (int i = 0; i < 4; ++i) {
        if (!scene.lights[i].enabled) {
            continue;
        }

        if (scene.lights[i].type == 0) {
            color += pointLight(i, inPosition, normal);
        }
        else {
            color += spotLight(i, inPosition, normal);
        }
    }

    outColor = min(color, 1.0);
}
