void PointLight(const in Light light,
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

    float attenuation = 1.0 / light.attenuation.x
            + light.attenuation.y * lightDistance
            + light.attenuation.z * lightDistance * lightDistance;
            
    float nDotL = max(0.0, dot(normal, lightDirection));

    ambient += light.ambient * attenuation;
    diffuse += light.diffuse * nDotL * attenuation;

    if (nDotL > 0.0) {
        vec4 viewVector = normalize(mv * position);
        vec3 halfVector = normalize(light.position - vec3(viewVector));    
        float nDotH = max(0.0, dot(normal, halfVector));
        specular += max(0.0, pow(nDotH, material.shininess)) * light.specular * attenuation;
    }
}


