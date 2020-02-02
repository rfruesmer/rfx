struct Light {
    int type;
    vec3 direction;
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 attenuationFactors;
    float spotExponent;
    float spotCutoff;
};
