//
// This fragment shader defines a reference implementation for Physically Based Shading of
// a microfacet surface material defined by a glTF model.
//
// References:
// [1] Real Shading in Unreal Engine 4
//     http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// [2] Physically Based Shading at Disney
//     http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
// [3] README.md - Environment Maps
//     https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
// [4] "An Inexpensive BRDF Model for Physically based Rendering" by Christophe Schlick
//     https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf
// [5] "KHR_materials_clearcoat"
//     https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat

#version 450
#rfx

precision highp float;

#include <functions.glsl>
#include <punctual.glsl>
#include <brdf.glsl>
#include <tonemapping.glsl>

layout(set = 0, binding = 0)
uniform SceneData {
    mat4 viewMatrix;
    mat4 projMatrix;
    mat4 viewProjMatrix;
    vec3 cameraPosition;
    float pad;
} scene;

layout(set = 1, binding = 0)
uniform ShaderData {
    Light lights[MAX_LIGHTS];
} shader;

layout(set = 2, binding = 0)
uniform MaterialData {
    vec4 baseColor;
    float metallicFactor;
    float roughnessFactor;
    float pad0;
    float pad1;
} material;

layout(set = 3, binding = 0)
uniform MeshData {
    mat4 modelMatrix;
} mesh;


layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

// ---------------------------------------------------------------------------------------------------------------------

struct MaterialInfo
{
    float ior;
    float perceptualRoughness;      // roughness value, as authored by the model creator (input to shader)
    vec3 f0;                        // full reflectance color (n incidence angle)

    float alphaRoughness;           // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 c_diff;

    vec3 f90;                       // reflectance color at grazing angle
    float metallic;

    vec3 baseColor;

    float sheenRoughnessFactor;
    vec3 sheenColorFactor;

    vec3 clearcoatF0;
    vec3 clearcoatF90;
    float clearcoatFactor;
    vec3 clearcoatNormal;
    float clearcoatRoughness;

// KHR_materials_specular
    float specularWeight; // product of specularFactor and specularTexture.a

    float transmissionFactor;

    float thickness;
    vec3 attenuationColor;
    float attenuationDistance;
};

// ---------------------------------------------------------------------------------------------------------------------

struct NormalInfo {
    vec3 ng;   // Geometry normal
    vec3 t;    // Geometry tangent
    vec3 b;    // Geometry bitangent
    vec3 n;    // Shading normal
    vec3 ntex; // Normal from texture, scaling is accounted for.
};

// ---------------------------------------------------------------------------------------------------------------------

vec4 getVertexColor()
{
    vec4 color = vec4(1.0);

#ifdef HAS_COLOR_0_VEC3
    color.rgb = v_Color.rgb;
#endif
#ifdef HAS_COLOR_0_VEC4
    color = v_Color;
#endif

    return color;
}

// ---------------------------------------------------------------------------------------------------------------------

vec4 getBaseColor()
{
    vec4 baseColor = vec4(1);

#if defined(MATERIAL_SPECULARGLOSSINESS)
    baseColor = u_DiffuseFactor;
#elif defined(MATERIAL_METALLICROUGHNESS)
    baseColor = material.baseColor;
#endif

#if defined(MATERIAL_SPECULARGLOSSINESS) && defined(HAS_DIFFUSE_MAP)
    baseColor *= texture(u_DiffuseSampler, getDiffuseUV());
#elif defined(MATERIAL_METALLICROUGHNESS) && defined(HAS_BASE_COLOR_MAP)
    baseColor *= texture(u_BaseColorSampler, getBaseColorUV());
#endif

    return baseColor * getVertexColor();
}

// ---------------------------------------------------------------------------------------------------------------------

//vec2 getNormalUV()
//{
//    vec3 uv = vec3(u_NormalUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
//
//#ifdef HAS_NORMAL_UV_TRANSFORM
//    uv = u_NormalUVTransform * uv;
//#endif
//
//    return uv.xy;
//}

// ---------------------------------------------------------------------------------------------------------------------

// Get normal, tangent and bitangent vectors.
//NormalInfo getNormalInfo(vec3 v)
//{
//    vec2 UV = getNormalUV();
//    vec3 uv_dx = dFdx(vec3(UV, 0.0));
//    vec3 uv_dy = dFdy(vec3(UV, 0.0));
//
//    vec3 t_ = (uv_dy.t * dFdx(v_Position) - uv_dx.t * dFdy(v_Position)) /
//              (uv_dx.s * uv_dy.t - uv_dy.s * uv_dx.t);
//
//    vec3 n, t, b, ng;
//
//    // Compute geometrical TBN:
//#ifdef HAS_NORMAL_VEC3
//#ifdef HAS_TANGENT_VEC4
//    // Trivial TBN computation, present as vertex attribute.
//    // Normalize eigenvectors as matrix is linearly interpolated.
//    t = normalize(v_TBN[0]);
//    b = normalize(v_TBN[1]);
//    ng = normalize(v_TBN[2]);
//#else
//    // Normals are either present as vertex attributes or approximated.
//    ng = normalize(v_Normal);
//    t = normalize(t_ - ng * dot(ng, t_));
//    b = cross(ng, t);
//#endif
//#else
//    ng = normalize(cross(dFdx(v_Position), dFdy(v_Position)));
//    t = normalize(t_ - ng * dot(ng, t_));
//    b = cross(ng, t);
//#endif
//
//    // For a back-facing surface, the tangential basis vectors are negated.
//    if (gl_FrontFacing == false)
//    {
//        t *= -1.0;
//        b *= -1.0;
//        ng *= -1.0;
//    }
//
//    // Compute normals:
//    NormalInfo info;
//    info.ng = ng;
//#ifdef HAS_NORMAL_MAP
//    info.ntex = texture(u_NormalSampler, UV).rgb * 2.0 - vec3(1.0);
//    info.ntex *= vec3(u_NormalScale, u_NormalScale, 1.0);
//    info.ntex = normalize(info.ntex);
//    info.n = normalize(mat3(t, b, ng) * info.ntex);
//#else
//    info.n = ng;
//#endif
//    info.t = t;
//    info.b = b;
//    return info;
//}

// ---------------------------------------------------------------------------------------------------------------------

vec3 getNormal()
{
#ifdef HAS_NORMAL_MAP
#endif

#ifdef HAS_NORMAL_VEC3
    vec3 N = inNormal;
#endif

    return normalize(N);
}

// ---------------------------------------------------------------------------------------------------------------------

float clampedDot(vec3 x, vec3 y)
{
    return clamp(dot(x, y), 0.0, 1.0);
}

// ---------------------------------------------------------------------------------------------------------------------

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 linearTosRGB(vec3 color)
{
    return pow(color, vec3(INV_GAMMA));
}

// ---------------------------------------------------------------------------------------------------------------------

vec3 toneMap(vec3 color)
{
//    color *= u_Exposure;

#ifdef TONEMAP_ACES_NARKOWICZ
    color = toneMapACES_Narkowicz(color);
#endif

#ifdef TONEMAP_ACES_HILL
    color = toneMapACES_Hill(color);
#endif

#ifdef TONEMAP_ACES_HILL_EXPOSURE_BOOST
    // boost exposure as discussed in https://github.com/mrdoob/three.js/pull/19621
    // this factor is based on the exposure correction of Krzysztof Narkowicz in his
    // implemetation of ACES tone mapping
    color /= 0.6;
    color = toneMapACES_Hill(color);
#endif

    return linearTosRGB(color);
}

// ---------------------------------------------------------------------------------------------------------------------

#ifdef MATERIAL_METALLICROUGHNESS
MaterialInfo getMetallicRoughnessInfo(MaterialInfo info)
{
    info.metallic = material.metallicFactor;
    info.perceptualRoughness = material.roughnessFactor;

#ifdef HAS_METALLIC_ROUGHNESS_MAP
    // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
    // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
    vec4 mrSample = texture(u_MetallicRoughnessSampler, getMetallicRoughnessUV());
    info.perceptualRoughness *= mrSample.g;
    info.metallic *= mrSample.b;
#endif

    // Achromatic f0 based on IOR.
    info.c_diff = mix(info.baseColor.rgb,  vec3(0), info.metallic);
    info.f0 = mix(info.f0, info.baseColor.rgb, info.metallic);

    return info;
}
#endif

// ---------------------------------------------------------------------------------------------------------------------

void main() {
    vec4 baseColor = getBaseColor();

    vec3 v = normalize(scene.cameraPosition - inPosition);
//    NormalInfo normalInfo = getNormalInfo(v);
//    vec3 n = normalInfo.n;
//    vec3 t = normalInfo.t;
//    vec3 b = normalInfo.b;
//
//    float NdotV = clampedDot(n, v);
//    float TdotV = clampedDot(t, v);
//    float BdotV = clampedDot(b, v);
    vec3 n = getNormal();

    MaterialInfo materialInfo;
    materialInfo.baseColor = baseColor.rgb;

    // The default index of refraction of 1.5 yields a dielectric normal incidence reflectance of 0.04.
    materialInfo.ior = 1.5;
    materialInfo.f0 = vec3(0.04);
    materialInfo.specularWeight = 1.0;


#ifdef MATERIAL_METALLICROUGHNESS
    materialInfo = getMetallicRoughnessInfo(materialInfo);
#endif

    materialInfo.perceptualRoughness = clamp(materialInfo.perceptualRoughness, 0.0, 1.0);
    materialInfo.metallic = clamp(materialInfo.metallic, 0.0, 1.0);

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness.
    materialInfo.alphaRoughness = materialInfo.perceptualRoughness * materialInfo.perceptualRoughness;

    // Compute reflectance.
    float reflectance = max(max(materialInfo.f0.r, materialInfo.f0.g), materialInfo.f0.b);

    // Anything less than 2% is physically impossible and is instead considered to be shadowing. Compare to "Real-Time-Rendering" 4th editon on page 325.
    materialInfo.f90 = vec3(1.0);

    // LIGHTING
    vec3 f_specular = vec3(0.0);
    vec3 f_diffuse = vec3(0.0);
    vec3 f_emissive = vec3(0.0);
    vec3 f_clearcoat = vec3(0.0);
    vec3 f_sheen = vec3(0.0);
    vec3 f_transmission = vec3(0.0);

    float albedoSheenScaling = 1.0;


#ifdef USE_PUNCTUAL

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        Light light = shader.lights[i];
        if (!light.enabled) {
            continue;
        }

        vec3 pointToLight;
        if (light.type != DIRECTIONAL_LIGHT) {
            pointToLight = light.position - inPosition;
        }
        else  {
            pointToLight = -light.direction;
        }

        // BSTF
        vec3 l = normalize(pointToLight);   // Direction from surface point to light
        vec3 h = normalize(l + v);          // Direction of the vector between l and v, called halfway vector
        float NdotL = clampedDot(n, l);
        float NdotV = clampedDot(n, v);
        float NdotH = clampedDot(n, h);
        float LdotH = clampedDot(l, h);
        float VdotH = clampedDot(v, h);
        if (NdotL > 0.0 || NdotV > 0.0)
        {
            // Calculation of analytical light
            // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
            vec3 intensity = getLightIntensity(light, pointToLight);
            f_diffuse += intensity * NdotL *  BRDF_lambertian(materialInfo.f0, materialInfo.f90, materialInfo.c_diff, materialInfo.specularWeight, VdotH);
            f_specular += intensity * NdotL * BRDF_specularGGX(materialInfo.f0, materialInfo.f90, materialInfo.alphaRoughness, materialInfo.specularWeight, VdotH, NdotL, NdotV, NdotH);
        }
    }

#endif

    vec3 color = f_emissive + f_diffuse + f_specular;
//    color = f_sheen + color * albedoSheenScaling;
//    color = color * (1.0 - clearcoatFactor * clearcoatFresnel) + f_clearcoat;

#ifdef LINEAR_OUTPUT
    outColor = vec4(color.rgb, baseColor.a);
#else
    outColor = vec4(toneMap(color), baseColor.a);
#endif
}

// ---------------------------------------------------------------------------------------------------------------------
