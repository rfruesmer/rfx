#version 450

#define MAX_LIGHTS 4

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

    vec3 camPos;
    float padding;

    // Light
    Light lights[4];

    int useNormalMap;
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

layout(set = 2, binding = 1)
uniform sampler2D baseColorTexture;

layout(set = 2, binding = 2)
uniform sampler2D normalMap;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangentCamPos;
layout(location = 4) in vec3 inTangentPosition;
layout(location = 5) in vec3 inTangentLightPos[MAX_LIGHTS];

layout(location = 0) out vec3 outColor;


// ---------------------------------------------------------------------------------------------------------------------

vec3 pointLightNM(int index, vec3 position, vec3 normal)
{
    vec3 baseColor = texture(baseColorTexture, inTexCoord).rgb;

    vec3 lightDirection = inTangentLightPos[index] - position;
    float lightDistance = length(lightDirection);
    lightDirection /= lightDistance;

    float attenuation = 1.0;
    if (scene.lights[index].range > 0.0) {
        attenuation = max(min(1.0f - (lightDistance / scene.lights[index].range), 1), 0) / lightDistance;
    }

    float nDotL = max(0.0, dot(normal, lightDirection));
    vec3 diffuse = baseColor * nDotL;

    vec3 specular = vec3(0.0);
    if (nDotL > 0.0 && material.shininess > 0.0) {
        vec3 eyeDirection = normalize(inTangentCamPos - position);
        vec3 halfVector = normalize(eyeDirection + lightDirection);
        float nDotH = max(0.0, dot(normal, halfVector));
        specular = baseColor * material.specular * pow(nDotH, material.shininess);
    }

    return scene.lights[index].color * (diffuse + specular) * attenuation;
}

// ---------------------------------------------------------------------------------------------------------------------

vec3 pointLight(int index, vec3 position, vec3 normal)
{
    vec3 baseColor = texture(baseColorTexture, inTexCoord).rgb;

    vec3 lightDirection = scene.lights[index].position - position;
    float lightDistance = length(lightDirection);
    lightDirection /= lightDistance;

    float attenuation = 1.0;
    if (scene.lights[index].range > 0.0) {
        attenuation = max(min(1.0f - (lightDistance / scene.lights[index].range), 1), 0) / lightDistance;
    }

    float nDotL = max(0.0, dot(normal, lightDirection));
    vec3 diffuse = baseColor * nDotL;

    vec3 specular = vec3(0.0);
    if (nDotL > 0.0 && material.shininess > 0.0) {
        vec3 eyeDirection = normalize(-position);
        vec3 halfVector = normalize(eyeDirection + lightDirection);
        float nDotH = max(0.0, dot(normal, halfVector));
        specular = baseColor * material.specular * pow(nDotH, material.shininess);
    }

    return scene.lights[index].color * (diffuse + specular) * attenuation;
}

// ---------------------------------------------------------------------------------------------------------------------

void main()
{
    vec3 color = vec3(0.0);

    vec3 normal;
    if (scene.useNormalMap != 0) {
        normal = texture(normalMap, inTexCoord).xyz * 2.0 - 1.0;
    }
    else {
        normal = normalize(inNormal);
    }

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (!scene.lights[i].enabled) {
            continue;
        }

        if (scene.lights[i].type != 0) {
            continue; // TODO: support for spot and other light types
        }

        if (scene.useNormalMap != 0) {
            color += pointLightNM(0, inTangentPosition, normal);
        }
        else {
            color += pointLight(0, inPosition, normal);
        }
    }

    outColor = min(color, 1.0);
}

// ---------------------------------------------------------------------------------------------------------------------
