void DirectionalLight(const in Light light,
                      const in vec3 normal,
                      const in Material material,
                      const in mat4 mv,
                      const in vec4 position,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular)
{
        
    float nDotL = max(0.0, dot(normal, light.position));

    ambient += light.ambient;
    diffuse += light.diffuse * nDotL;


    if (nDotL > 0.0) {
        vec4 viewVector = normalize(mv * position);
        vec3 halfVector = normalize(light.position - vec3(viewVector));    
        float nDotH = max(0.0, dot(normal, halfVector));
        specular += max(0.0, pow(nDotH, material.shininess)) * light.specular;
    }
}