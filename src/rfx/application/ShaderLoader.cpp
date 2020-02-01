#include "rfx/pch.h"
#include "rfx/application/ShaderLoader.h"
#include "rfx/graphics/shader/SPIR.h"
#include "rfx/core/FileUtil.h"


using namespace rfx;
using namespace std;
using namespace std::filesystem;

// ---------------------------------------------------------------------------------------------------------------------

ShaderLoader::ShaderLoader(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
    : graphicsDevice(graphicsDevice) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<VertexShader> ShaderLoader::loadVertexShader(
    const path& path, 
    const char* entryPoint,
    const VertexFormat& vertexFormat) const
{
    const VkPipelineShaderStageCreateInfo shaderStageCreateInfo = 
        loadInternal(path, VK_SHADER_STAGE_VERTEX_BIT, entryPoint);

    return graphicsDevice->createVertexShader(shaderStageCreateInfo, vertexFormat);
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineShaderStageCreateInfo ShaderLoader::loadInternal(
    const path& path, 
    VkShaderStageFlagBits stage,
    const char* entryPoint) const
{
    string shaderString;
    FileUtil::readTextFile(path, shaderString);

    insertIncludedFiles(path.parent_path(), shaderString);

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

void ShaderLoader::insertIncludedFiles(const path& currentShaderDirectoryPath, string& inoutShaderString) const
{
    smatch match;
    const std::regex regex("#include \\s*\"(.*)\"");

    while (regex_search(inoutShaderString, match, regex)) {
        const path relativeIncludedFilePath = match[1].str();
        insertIncludedFile(currentShaderDirectoryPath, relativeIncludedFilePath, 
            match.position(), match.length(), inoutShaderString);        
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderLoader::insertIncludedFile(
    const path& currentShaderDirectoryPath,
    const path& relativeIncludedFilePath, 
    size_t first,
    size_t length,
    string& inoutShaderString) const
{
    const path absoluteIncludedFilePath = currentShaderDirectoryPath / relativeIncludedFilePath;

    string includedShaderString;
    FileUtil::readTextFile(absoluteIncludedFilePath, includedShaderString);
    insertIncludedFiles(absoluteIncludedFilePath, includedShaderString);

    inoutShaderString.replace(first, length, includedShaderString);
}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<FragmentShader> ShaderLoader::loadFragmentShader(
    const path& path,
    const char* entryPoint) const
{
    const VkPipelineShaderStageCreateInfo shaderStageCreateInfo =
        loadInternal(path, VK_SHADER_STAGE_FRAGMENT_BIT, entryPoint);

    return graphicsDevice->createFragmentShader(shaderStageCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

