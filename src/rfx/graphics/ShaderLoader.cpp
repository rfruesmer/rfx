#include "rfx/pch.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/graphics/SPIR.h"
#include "rfx/core/FileUtil.h"

using namespace rfx;
using namespace std;

// ---------------------------------------------------------------------------------------------------------------------

ShaderLoader::ShaderLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : graphicsDevice(graphicsDevice) {}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineShaderStageCreateInfo ShaderLoader::load(
    const std::filesystem::path& path, 
    VkShaderStageFlagBits stage,
    const char* entryPoint)
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
