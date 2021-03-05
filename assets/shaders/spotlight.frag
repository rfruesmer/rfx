#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0)
uniform SceneData {
    // Camera
    mat4 viewMatrix;
    mat4 projMatrix;

    // Light
    vec3 lightPos;              // light position in eye coords
    float pad1;
    vec3 lightColor;
    float pad2;
    vec3 spotDirection;         // Direction of the spotlight in eye coords
    float spotExponent;         // Angular attenuation exponent
    float spotCutoff;           // Cutoff angle (0-90 in radians)
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


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inSpotDirection;

layout(location = 0) out vec3 outColor;


vec3 spotLight(vec3 eyePos, vec3 eyeNormal) {

    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    vec3 lightDirection = normalize(scene.lightPos - eyePos);
    float cosAngle = dot(-lightDirection, normalize(inSpotDirection));
    float angle = acos(cosAngle);
    float attenuation = 0.0;

    if (angle < scene.spotCutoff) {
        attenuation = pow(cosAngle, scene.spotExponent);
        float sDotN = max(dot(lightDirection, eyeNormal), 0.0);
        diffuse = vec3(material.baseColor) * sDotN;
        if (sDotN > 0.0) {
            vec3 v = normalize(-eyePos);
            vec3 h = normalize(v + lightDirection);
            specular = vec3(material.baseColor) * pow(max(dot(h, eyeNormal), 0.0), material.shininess);
        }
    }

    return attenuation * (diffuse + specular);
}


void main() {

    outColor = spotLight(inPosition, normalize(inNormal));
}