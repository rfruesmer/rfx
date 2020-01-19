#include "rfx/pch.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/graphics/SPIR.h"
#include "rfx/core/FileUtil.h"


using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

ShaderLoader::ShaderLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : graphicsDevice(graphicsDevice) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<VertexShader> ShaderLoader::loadVertexShader(const path& path, 
    const char* entryPoint,
    const VertexFormat& vertexFormat)
{
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = 
        loadInternal(path, VK_SHADER_STAGE_VERTEX_BIT, entryPoint);

    return make_shared<VertexShader>(graphicsDevice, shaderStageCreateInfo, vertexFormat);
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineShaderStageCreateInfo ShaderLoader::loadInternal(
    const path& path, 
    VkShaderStageFlagBits stage,
    const char* entryPoint) const
{
    string shaderString;
    FileUtil::readTextFile(path, shaderString);
    vector<unsigned int> shaderSPV;
    GLSLtoSPV(stage, shaderString.c_str(), shaderSPV);

    VkPipelineShaderStageCreateInfo shaderStage = {};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.pNext = nullptr;
    shaderStage.pSpecializationInfo = nullptr;
    shaderStage.flags = 0;
    shaderStage.stage = stage;
    shaderStage.pName = entryPoint;

    VkShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;
    shaderModuleCreateInfo.flags = 0;
    shaderModuleCreateInfo.codeSize = shaderSPV.size() * sizeof(unsigned int);
    shaderModuleCreateInfo.pCode = shaderSPV.data();

    shaderStage.module = graphicsDevice->createShaderModule(shaderModuleCreateInfo);

    return shaderStage;
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<FragmentShader> ShaderLoader::loadFragmentShader(const path& path,
    const char* entryPoint)
{
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo =
        loadInternal(path, VK_SHADER_STAGE_FRAGMENT_BIT, entryPoint);

    return make_shared<FragmentShader>(graphicsDevice, shaderStageCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

