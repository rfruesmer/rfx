#pragma once

#include "rfx/graphics/shader/VertexShader.h"
#include "rfx/graphics/shader/FragmentShader.h"
#include "rfx/graphics/VertexFormat.h"


namespace rfx
{

class ShaderProgram
{
public:
    ShaderProgram(const std::shared_ptr<VertexShader>& vertexShader,
        const std::shared_ptr<FragmentShader>& fragmentShader);

    const VertexFormat& getVertexFormat() const;
    const std::shared_ptr<VertexShader>& getVertexShader() const;
    const std::shared_ptr<FragmentShader>& getFragmentShader() const;

    const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const;

private:
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
};    

} // namespace rfx
