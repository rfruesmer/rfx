#include "rfx/pch.h"
#include "rfx/graphics/ShaderLoader.h"
#include "rfx/graphics/ShaderCompiler.h"
#include "rfx/common/FileUtil.h"
#include "rfx/common/Logger.h"

using namespace rfx;
using namespace std;
using namespace filesystem;

// ---------------------------------------------------------------------------------------------------------------------

ShaderLoader::ShaderLoader(shared_ptr<GraphicsDevice> graphicsDevice)
    : graphicsDevice(move(graphicsDevice)) {}

// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<VertexShader> ShaderLoader::loadVertexShader(
    const filesystem::path& path,
    const char* entryPoint,
    const VertexFormat& vertexFormat,
    const vector<string>& defines,
    const vector<string>& inputs,
    const vector<string>& outputs) const
{
    const VkPipelineShaderStageCreateInfo shaderStageCreateInfo =
        loadInternal(path, VK_SHADER_STAGE_VERTEX_BIT, entryPoint, defines, inputs, outputs);

    return make_shared<VertexShader>(
        graphicsDevice->getLogicalDevice(),
        shaderStageCreateInfo,
        vertexFormat);
}

// ---------------------------------------------------------------------------------------------------------------------

VkPipelineShaderStageCreateInfo ShaderLoader::loadInternal(
    const path& path,
    VkShaderStageFlagBits stage,
    const char* entryPoint,
    const vector<string>& defines,
    const vector<string>& inputs,
    const vector<string>& outputs) const
{
    string shaderString;
    FileUtil::readTextFile(path, shaderString);

    vector<uint32_t> shaderSPV;

    const string extension = path.extension().string();
    if (extension != ".spv") {
        insertIncludedFiles(path.parent_path(), shaderString);
        configure(defines, inputs, outputs, shaderString);
        GLSLtoSPV(stage, shaderString.c_str(), shaderSPV);
    }
    else {
        shaderSPV.resize(shaderString.size() / 4);
        memcpy(shaderSPV.data(), shaderString.c_str(), shaderString.size() * sizeof(char));
    }


    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = shaderSPV.size() * sizeof(uint32_t),
        .pCode = shaderSPV.data()
    };

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    const VkResult result = vkCreateShaderModule(graphicsDevice->getLogicalDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule);
    RFX_CHECK_STATE(result == VK_SUCCESS && shaderModule != VK_NULL_HANDLE, "Failed to create shader module for: " + path.string());

    VkPipelineShaderStageCreateInfo shaderStage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stage,
        .module = shaderModule,
        .pName = entryPoint,
        .pSpecializationInfo = nullptr
    };

    return shaderStage;
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderLoader::configure(
    const vector<string>& defines,
    const vector<string>& inputs,
    const vector<string>& outputs,
    string& inoutShaderString)
{
    smatch match;
    const regex regex("#rfx");
    if (!regex_search(inoutShaderString, match, regex)) {
        return;
    }

    string insertString;
    for (const string& define : defines) {
        insertString.append("#define " + define + "\n");
    }
    insertString.append("\n");

    for (const string& input : inputs) {
        insertString.append(input + "\n");
    }
    insertString.append("\n");

    for (const string& output : outputs) {
        insertString.append(output + "\n");
    }
    insertString.append("\n");

    inoutShaderString.replace(match.position(), match.length(), insertString);

//    RFX_LOG_INFO << inoutShaderString;
}

// ---------------------------------------------------------------------------------------------------------------------

void ShaderLoader::insertIncludedFiles(const path& currentShaderDirectoryPath, string& inoutShaderString) const
{
    smatch match;
    const regex regex("#include \\s*\"(.*)\"");

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
    const char* entryPoint,
    const vector<string>& defines,
    const vector<string>& inputs) const
{
    const VkPipelineShaderStageCreateInfo shaderStageCreateInfo =
        loadInternal(path, VK_SHADER_STAGE_FRAGMENT_BIT, entryPoint, defines, inputs, vector<string>());

    return make_shared<FragmentShader>(
        graphicsDevice->getLogicalDevice(),
        shaderStageCreateInfo);
}

// ---------------------------------------------------------------------------------------------------------------------

