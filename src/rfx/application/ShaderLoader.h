#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/VertexShader.h"
#include "rfx/graphics/FragmentShader.h"

namespace rfx {

class ShaderLoader
{
public:
    explicit ShaderLoader(std::shared_ptr<GraphicsDevice> graphicsDevice);

    std::shared_ptr<VertexShader> loadVertexShader(
        const std::filesystem::path& path,
        const char* entryPoint,
        const VertexFormat& vertexFormat,
        const std::vector<std::string>& defines,
        const std::vector<std::string>& inputs,
        const std::vector<std::string>& outputs) const;

    std::shared_ptr<FragmentShader> loadFragmentShader(
        const std::filesystem::path& path,
        const char* entryPoint,
        const std::vector<std::string>& defines,
        const std::vector<std::string>& inputs) const;

private:
    VkPipelineShaderStageCreateInfo loadInternal(
        const std::filesystem::path& path,
        VkShaderStageFlagBits stage,
        const char* entryPoint,
        const std::vector<std::string>& defines,
        const std::vector<std::string>& inputs,
        const std::vector<std::string>& outputs) const;

    static void configure(
        const std::vector<std::string>& defines,
        const std::vector<std::string>& inputs,
        const std::vector<std::string>& outputs,
        std::string& inoutShaderString) ;

    void insertIncludedFiles(
        const std::filesystem::path& currentShaderDirectoryPath,
        std::string& inoutShaderString) const;

    void insertIncludedFile(const std::filesystem::path& currentShaderDirectoryPath, 
        const std::filesystem::path& relativeIncludedFilePath,
        size_t first, 
        size_t length,
        std::string& inoutShaderString) const;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
};
    
} // namespace rfx
