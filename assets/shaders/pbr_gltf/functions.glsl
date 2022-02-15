
// ---------------------------------------------------------------------------------------------------------------------

const float M_PI = 3.141592653589793;

// ---------------------------------------------------------------------------------------------------------------------

vec4 getVertexColor()
{
    vec4 color = vec4(1.0);

#ifdef HAS_COLOR_VEC3
    color.rgb = inColor.rgb;
#endif
#ifdef HAS_COLOR_VEC4
    color = inColor;
#endif

    return color;
}

// ---------------------------------------------------------------------------------------------------------------------