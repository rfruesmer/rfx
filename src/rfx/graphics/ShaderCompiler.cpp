#include "rfx/pch.h"
#include "rfx/graphics/ShaderCompiler.h"
#include "rfx/common/StringUtil.h"


using namespace rfx;
using namespace glslang;
using namespace std;

namespace rfx {

// ---------------------------------------------------------------------------------------------------------------------

EShLanguage findLanguage(VkShaderStageFlagBits shaderType);

void initResources(TBuiltInResource& Resources);

// ---------------------------------------------------------------------------------------------------------------------

void GLSLtoSPV(
    const VkShaderStageFlagBits shaderType,
    const char* shaderString,
    vector<unsigned int>& spirv)
{
    EShLanguage language = findLanguage(shaderType);
    TShader shader(language);

    TBuiltInResource resources = {};
    initResources(resources);

    const char* shaderStrings[1];
    shaderStrings[0] = shaderString;
    shader.setStrings(shaderStrings, 1);

    auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
    if (!shader.parse(&resources, 450, false, messages)) {
        RFX_THROW(StringUtil::trimRight(string(shader.getInfoLog()))
            + "\nInfo Log: " + string(shader.getInfoDebugLog()));
    }

    TProgram program;
    program.addShader(&shader);

    if (!program.link(messages)) {
        RFX_THROW(StringUtil::trimRight(string(shader.getInfoLog())) 
            + "\nInfo Log: " + string(shader.getInfoDebugLog()));
    }

    TIntermediate* pIntermediate = program.getIntermediate(language);
    GlslangToSpv(*pIntermediate, spirv);
}

// ---------------------------------------------------------------------------------------------------------------------

EShLanguage findLanguage(const VkShaderStageFlagBits shaderType)
{
    static const unordered_map<VkShaderStageFlagBits, EShLanguage> languageMap = {
        { VK_SHADER_STAGE_VERTEX_BIT, EShLangVertex },
        { VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, EShLangTessControl },
        { VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, EShLangTessEvaluation },
        { VK_SHADER_STAGE_GEOMETRY_BIT, EShLangGeometry },
        { VK_SHADER_STAGE_FRAGMENT_BIT, EShLangFragment },
        { VK_SHADER_STAGE_COMPUTE_BIT, EShLangCompute }
    };

    auto it = languageMap.find(shaderType);
    RFX_CHECK_ARGUMENT(it != languageMap.end());

    return it != languageMap.end() ? it->second : EShLangVertex;
}

// ---------------------------------------------------------------------------------------------------------------------

void initResources(TBuiltInResource& resources)
{
    resources = {
        .maxLights = 32,
        .maxClipPlanes = 6,
        .maxTextureUnits = 32,
        .maxTextureCoords = 32,
        .maxVertexAttribs = 64,
        .maxVertexUniformComponents = 4096,
        .maxVaryingFloats = 64,
        .maxVertexTextureImageUnits = 32,
        .maxCombinedTextureImageUnits = 80,
        .maxTextureImageUnits = 32,
        .maxFragmentUniformComponents = 4096,
        .maxDrawBuffers = 32,
        .maxVertexUniformVectors = 128,
        .maxVaryingVectors = 8,
        .maxFragmentUniformVectors = 16,
        .maxVertexOutputVectors = 16,
        .maxFragmentInputVectors = 15,
        .minProgramTexelOffset = -8,
        .maxProgramTexelOffset = 7,
        .maxClipDistances = 8,
        .maxComputeWorkGroupCountX = 65535,
        .maxComputeWorkGroupCountY = 65535,
        .maxComputeWorkGroupCountZ = 65535,
        .maxComputeWorkGroupSizeX = 1024,
        .maxComputeWorkGroupSizeY = 1024,
        .maxComputeWorkGroupSizeZ = 64,
        .maxComputeUniformComponents = 1024,
        .maxComputeTextureImageUnits = 16,
        .maxComputeImageUniforms = 8,
        .maxComputeAtomicCounters = 8,
        .maxComputeAtomicCounterBuffers = 1,
        .maxVaryingComponents = 60,
        .maxVertexOutputComponents = 64,
        .maxGeometryInputComponents = 64,
        .maxGeometryOutputComponents = 128,
        .maxFragmentInputComponents = 128,
        .maxImageUnits = 8,
        .maxCombinedImageUnitsAndFragmentOutputs = 8,
        .maxCombinedShaderOutputResources = 8,
        .maxImageSamples = 0,
        .maxVertexImageUniforms = 0,
        .maxTessControlImageUniforms = 0,
        .maxTessEvaluationImageUniforms = 0,
        .maxGeometryImageUniforms = 0,
        .maxFragmentImageUniforms = 8,
        .maxCombinedImageUniforms = 8,
        .maxGeometryTextureImageUnits = 16,
        .maxGeometryOutputVertices = 256,
        .maxGeometryTotalOutputComponents = 1024,
        .maxGeometryUniformComponents = 1024,
        .maxGeometryVaryingComponents = 64,
        .maxTessControlInputComponents = 128,
        .maxTessControlOutputComponents = 128,
        .maxTessControlTextureImageUnits = 16,
        .maxTessControlUniformComponents = 1024,
        .maxTessControlTotalOutputComponents = 4096,
        .maxTessEvaluationInputComponents = 128,
        .maxTessEvaluationOutputComponents = 128,
        .maxTessEvaluationTextureImageUnits = 16,
        .maxTessEvaluationUniformComponents = 1024,
        .maxTessPatchComponents = 120,
        .maxPatchVertices = 32,
        .maxTessGenLevel = 64,
        .maxViewports = 16,
        .maxVertexAtomicCounters = 0,
        .maxTessControlAtomicCounters = 0,
        .maxTessEvaluationAtomicCounters = 0,
        .maxGeometryAtomicCounters = 0,
        .maxFragmentAtomicCounters = 8,
        .maxCombinedAtomicCounters = 8,
        .maxAtomicCounterBindings = 1,
        .maxVertexAtomicCounterBuffers = 0,
        .maxTessControlAtomicCounterBuffers = 0,
        .maxTessEvaluationAtomicCounterBuffers = 0,
        .maxGeometryAtomicCounterBuffers = 0,
        .maxFragmentAtomicCounterBuffers = 1,
        .maxCombinedAtomicCounterBuffers = 1,
        .maxAtomicCounterBufferSize = 16384,
        .maxTransformFeedbackBuffers = 4,
        .maxTransformFeedbackInterleavedComponents = 64,
        .maxCullDistances = 8,
        .maxCombinedClipAndCullDistances = 8,
        .maxSamples = 4,
        .maxMeshOutputVerticesNV = 256,
        .maxMeshOutputPrimitivesNV = 512,
        .maxMeshWorkGroupSizeX_NV = 32,
        .maxMeshWorkGroupSizeY_NV = 1,
        .maxMeshWorkGroupSizeZ_NV = 1,
        .maxTaskWorkGroupSizeX_NV = 32,
        .maxTaskWorkGroupSizeY_NV = 1,
        .maxTaskWorkGroupSizeZ_NV = 1,
        .maxMeshViewCountNV = 4,
        .limits = {
            .nonInductiveForLoops = true,
            .whileLoops = true,
            .doWhileLoops = true,
            .generalUniformIndexing = true,
            .generalAttributeMatrixVectorIndexing = true,
            .generalVaryingIndexing = true,
            .generalSamplerIndexing = true,
            .generalVariableIndexing = true,
            .generalConstantMatrixVectorIndexing = true
        }
    };
}

// ---------------------------------------------------------------------------------------------------------------------
} // namespace rfx