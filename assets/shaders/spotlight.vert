#version 450

layout(set = 0, binding = 0)
uniform SceneData {
    // Camera
    mat4 viewMatrix;
    mat4 projMatrix;

    // Light
    vec4 lightPos;              // light position in eye coords
    vec4 La;                    // Ambient light intensity
    vec4 Ld;                    // Diffuse light intensity
    vec4 Ls;                    // Specular light intensity
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

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outSpotDirection;

void main() {
    mat4 modelViewMatrix = scene.viewMatrix * mesh.modelMatrix;
    mat4 modelViewProjMatrix = scene.projMatrix * modelViewMatrix;
    mat3 normalMatrix = mat3(vec3(modelViewMatrix[0]), vec3(modelViewMatrix[1]), vec3(modelViewMatrix[2]));
    vec3 normal = normalize(normalMatrix * inNormal);

    gl_Position =  modelViewProjMatrix * vec4(inPosition, 1.0);

    outPosition = (modelViewMatrix * vec4(inPosition, 1.0)).xyz;
    outNormal = normal;
    outSpotDirection = normalMatrix * scene.spotDirection;
}
