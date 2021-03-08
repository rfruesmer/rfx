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
    vec3 specularFactor;
    float shininess;
} material;

layout(set = 2, binding = 1)
uniform sampler2D texSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in mat3 inNormalMatrix;

layout(location = 0) out vec3 outColor;


vec3 pointLight(int index, vec3 eyePos, vec3 eyeNormal) {

    vec3 texColor = texture(texSampler, inTexCoord).rgb;

    vec3 lightDirection = scene.lights[index].position - eyePos;
    float distance = length(lightDirection);
    lightDirection /= distance;

    float attenuation = 1.0;
    if (scene.lights[index].range > 0.0) {
        attenuation = max(min(1.0f - (distance / scene.lights[index].range), 1), 0) / distance;
    }

    float sDotN = max(0.0, dot(lightDirection, eyeNormal));
    vec3 diffuse = texColor * sDotN;
    vec3 specular = vec3(0.0);

    if (material.shininess > 0.0 && sDotN > 0.0) {
        vec3 v = normalize(-eyePos);
        vec3 h = normalize(v + lightDirection);
        specular = material.specularFactor
            * pow(max(dot(h, eyeNormal), 0.0), material.shininess);
    }

    return scene.lights[index].color * (diffuse + specular) * attenuation;
}

vec3 spotLight(int index, vec3 eyePos, vec3 eyeNormal) {

    vec3 texColor = texture(texSampler, inTexCoord).rgb;

    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    vec3 lightDirection = normalize(scene.lights[index].position - eyePos);
    vec3 spotDirection = normalize(inNormalMatrix * scene.lights[index].direction);
    float spotCos = dot(lightDirection, -spotDirection);
    float attenuation = 0.0;

    if (spotCos > scene.lights[index].spotCosOuterConeAngle) {
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
        float sDotN = max(0.0, dot(eyeNormal, lightDirection));
        diffuse = texColor * sDotN;

        if (material.shininess > 0.0 && sDotN > 0.0) {
            vec3 v = normalize(-eyePos);
            vec3 h = normalize(v + lightDirection);
            specular = texColor * material.specularFactor
                * pow(max(dot(h, eyeNormal), 0.0), material.shininess);
        }
    }

    return scene.lights[index].color * (diffuse + specular) * attenuation;
}

void main() {
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

    outColor = color;
}