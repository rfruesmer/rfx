#pragma once

#include "rfx/graphics/GraphicsDevice.h"
#include "rfx/graphics/ShaderProgram.h"


namespace rfx {

class ShaderLoader
{
public:
    explicit ShaderLoader(std::shared_ptr<GraphicsDevice> graphicsDevice);

    [[nodiscard]]
    ShaderProgramPtr loadProgram(
        const std::filesystem::path& vertexShaderPath,
        const std::filesystem::path& fragmentShaderPath,
        const VertexFormat& vertexFormat) const;

    [[nodiscard]]
    ShaderProgramPtr loadProgram(
        const std::filesystem::path& vertexShaderPath,
        const std::filesystem::path& fragmentShaderPath,
        const VertexFormat& vertexFormat,
        const std::vector<std::string>& defines,
        const std::vector<std::string>& vertexShaderInputs,
        const std::vector<std::string>& vertexShaderOutputs,
        const std::vector<std::string>& fragmentShaderInputs) const;

    [[nodiscard]]
    VertexShaderPtr loadVertexShader(
        const std::filesystem::path& path,
        const char* entryPoint,
        const VertexFormat& vertexFormat,
        const std::vector<std::string>& defines,
        const std::vector<std::string>& inputs,
        const std::vector<std::string>& outputs) const;

    [[nodiscard]]
    FragmentShaderPtr loadFragmentShader(
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
