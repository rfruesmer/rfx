#pragma once


namespace rfx {

class Shader
{
public:
    Shader(VkDevice vkDevice, const VkPipelineShaderStageCreateInfo& stageCreateInfo);

    virtual ~Shader();

    void dispose();

    const VkPipelineShaderStageCreateInfo& getStageCreateInfo() const;
    VkShaderModule getModule() const;

private:
    VkDevice vkDevice = nullptr;
    VkPipelineShaderStageCreateInfo stageCreateInfo = {};
};
    
} // namespace rfx
