#version 450

layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewMatrix;
    mat4 projMatrix;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    vec3 lightPos;
    float pad;
    vec3 lightColor;
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    vec3 specularFactor;
    float shininess;
} material;

layout(set = 3, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outColor;


vec3 pointLight(vec3 eyePos, vec3 eyeNormal)
{
    vec3 lightDirection = normalize(shader.lightPos - eyePos);
    float sDotN = max(dot(lightDirection, eyeNormal), 0.0);
    vec3 diffuse = material.baseColor.xyz * sDotN;
    vec3 specular = vec3(0.0);

    if (sDotN > 0.0) {
        vec3 v = normalize(-eyePos);
        vec3 h = normalize(v + lightDirection);
        specular = material.specularFactor * pow(max(dot(h, eyeNormal), 0.0), material.shininess);
    }

    return shader.lightColor * (diffuse + specular);
}

void main()
{
    outColor = pointLight(inPosition, normalize(inNormal));
}