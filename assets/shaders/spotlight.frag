#version 450

layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewMatrix;
    mat4 projMatrix;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    vec3 lightPos;
    float pad1;
    vec3 lightColor;
    float pad2;
    vec3 spotDirection;
    float pad3;
    float innerConeAngle;
    float outerConeAngle;
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
layout(location = 2) in vec3 inSpotDirection;

layout(location = 0) out vec3 outColor;


vec3 spotLight(vec3 eyePos, vec3 eyeNormal) {

    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    vec3 lightDirection = normalize(shader.lightPos - eyePos);
    float cosAngle = dot(-lightDirection, normalize(inSpotDirection));
    float attenuation = 0.0;

    if (cosAngle > shader.outerConeAngle) {
        float epsilon = shader.innerConeAngle - shader.outerConeAngle;
        attenuation = clamp((cosAngle - shader.outerConeAngle) / epsilon, 0.0, 1.0);

        float sDotN = max(dot(lightDirection, eyeNormal), 0.0);
        diffuse = vec3(material.baseColor) * shader.lightColor * sDotN;

        if (sDotN > 0.0) {
            vec3 v = normalize(-eyePos);
            vec3 h = normalize(v + lightDirection);
            specular = material.specularFactor * shader.lightColor * pow(max(dot(h, eyeNormal), 0.0), material.shininess);
        }
    }

    return attenuation * (diffuse + specular);
}


void main()
{
    outColor = spotLight(inPosition, normalize(inNormal));
}