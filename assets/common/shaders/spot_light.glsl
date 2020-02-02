
void SpotLight(const in Light light,
                const in vec3 normal,
                const in Material material,
                const in mat4 mv,
                const in vec4 position,
                inout vec4 ambient,
                inout vec4 diffuse,
                inout vec4 specular)
{
    vec3 lightDirection = light.position - vec3(position);
    float lightDistance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    float attenuation = 1.0 / light.attenuationFactors.x
            + light.attenuationFactors.y * lightDistance
            + light.attenuationFactors.z * lightDistance * lightDistance;

    float spotDot = dot(-lightDirection, normalize(light.direction));
    if (spotDot < light.spotCutoff) {
        attenuation *= pow(spotDot, light.spotExponent);
    }
    else {
        attenuation = 0.0;
    }

    float nDotL = max(0.0, dot(normal, lightDirection));

    ambient += light.ambient * attenuation;
    diffuse += light.diffuse * attenuation * nDotL;

    if (nDotL > 0.0) {
        vec4 viewVector = normalize(mv * position);
        vec3 halfVector = normalize(light.position - vec3(viewVector));    
        float nDotH = max(0.0, dot(normal, halfVector));
        specular += max(0.0, pow(nDotH, material.shininess)) * light.specular * attenuation;
    }
}