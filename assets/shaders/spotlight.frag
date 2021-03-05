#version 450
#extension GL_ARB_separate_shader_objects : enable

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
layout(location = 2) in vec3 inSpotDirection;

layout(location = 0) out vec4 outColor;


//vec4 phong(vec3 eyePos, vec3 eyeNormal) {
//
//    vec3 ambient = vec3(scene.La * material.baseColor);
//
//    vec3 s = normalize(vec3(scene.lightPos) - eyePos);
//    float sDotN = max(dot(s, eyeNormal), 0.0);
//
//    vec3 diffuse = vec3(scene.Ld * material.baseColor) * sDotN;
//
//    vec3 specular = vec3(0.0);
//    if (sDotN > 0.0) {
//        vec3 v = normalize(-eyePos.xyz);
//        vec3 r = reflect(-s, eyeNormal);
//        specular = vec3(scene.Ls * material.baseColor) * pow(max(dot(r, v), 0.0), material.shininess);
//    }
//
//    return vec4(ambient + diffuse + specular, 1.0);
//}

vec3 blinnPhongSpot(vec3 eyePos, vec3 eyeNormal) {

    vec3 ambient = vec3(scene.La * material.baseColor);
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    vec3 s = normalize(vec3(scene.lightPos) - eyePos);
    float cosAngle = dot(-s, normalize(inSpotDirection.xyz));
    float angle = acos(cosAngle);
    float spotScale = 0.0;

    if (angle < scene.spotCutoff) {
        spotScale = pow(cosAngle, scene.spotExponent);
        float sDotN = max(dot(s, eyeNormal), 0.0);
        diffuse = (material.baseColor * scene.Ld * sDotN).xyz;
        if (sDotN > 0.0) {
            vec3 v = normalize(-eyePos.xyz);
            vec3 h = normalize(v + s);
            specular = (material.baseColor * scene.Ls * pow(max(dot(h, eyeNormal), 0.0), material.shininess)).xyz;
        }
    }

    return ambient + spotScale * (diffuse + specular);
}


void main() {

    outColor = vec4(blinnPhongSpot(inPosition, normalize(inNormal)), 1.0);
}