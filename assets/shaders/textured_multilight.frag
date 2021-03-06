#version 450

struct Light {
    vec3 position;          // light position in eye coords
    float pad1;
    vec3 color;
    float pad2;
    vec3 direction;
    float pad3;
    float exponent;
    float cutoff;
    int type;
    bool enabled;
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
    vec3 lightDirection = normalize(scene.lights[index].position - eyePos);
    float sDotN = max(dot(lightDirection, eyeNormal), 0.0);
    vec3 diffuse = texColor * sDotN;
    vec3 specular = vec3(0.0);

    if (sDotN > 0.0) {
        vec3 v = normalize(-eyePos);
        vec3 h = normalize(v + lightDirection);
        specular = vec3(0.0, 0.0, 0.0) * material.baseColor.xyz * pow(max(dot(h, eyeNormal), 0.0), material.shininess);
    }

    return scene.lights[index].color * (diffuse + specular);
}

vec3 spotLight(int index, vec3 eyePos, vec3 eyeNormal) {

    vec3 texColor = texture(texSampler, inTexCoord).rgb;
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    vec3 lightDirection = normalize(scene.lights[index].position - eyePos);
    vec3 spotDirection = inNormalMatrix * scene.lights[index].direction;
    float cosAngle = dot(-lightDirection, normalize(spotDirection));
    float angle = acos(cosAngle);
    float attenuation = 0.0;

    if (angle < scene.lights[index].cutoff) {
        attenuation = pow(cosAngle, scene.lights[index].exponent);
        float sDotN = max(dot(lightDirection, eyeNormal), 0.0);
        diffuse = texColor * sDotN;
        if (sDotN > 0.0) {
            vec3 v = normalize(-eyePos);
            vec3 h = normalize(v + lightDirection);
            specular = texColor * pow(max(dot(h, eyeNormal), 0.0), material.shininess);
        }
    }

    return attenuation * (diffuse + specular);
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