

// ---------------------------------------------------------------------------------------------------------------------

vec4 getBaseColor()
{
    vec4 baseColor = vec4(1);

#if defined(MATERIAL_SPECULARGLOSSINESS)
    baseColor = u_DiffuseFactor;
#elif defined(MATERIAL_METALLICROUGHNESS)
    baseColor = material.baseColorFactor;
#endif

#if defined(MATERIAL_SPECULARGLOSSINESS) && defined(HAS_DIFFUSE_MAP)
    baseColor *= texture(u_DiffuseSampler, getDiffuseUV());
#elif defined(MATERIAL_METALLICROUGHNESS) && defined(HAS_BASE_COLOR_MAP)
    baseColor *= texture(baseColorSampler, getBaseColorUV());
#endif

    return baseColor * getVertexColor();
}

// ---------------------------------------------------------------------------------------------------------------------
