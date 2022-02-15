
// Metallic Roughness Material

#ifdef MATERIAL_METALLICROUGHNESS

layout(set = 2, binding = 1)
uniform sampler2D baseColorSampler;

#ifdef HAS_TEXCOORD_VEC2

vec2 getBaseColorUV()
{
    vec3 uv = vec3(inTexCoord[0], 1.0);

//#ifdef HAS_BASECOLOR_UV_TRANSFORM
//    uv = u_BaseColorUVTransform * uv;
//#endif

    return uv.xy;
}

#endif // HAS_TEXCOORD_VEC2

#endif // MATERIAL_METALLICROUGHNESS


// Specular Glossiness Material

#ifdef MATERIAL_SPECULARGLOSSINESS

#endif