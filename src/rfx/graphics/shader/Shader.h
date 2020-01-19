#pragma once


namespace rfx
{

class Shader
{
public:
    explicit Shader(
        VkDevice vkDevice,
        const VulkanDeviceFunctionPtrs& vk, 
        const VkPipelineShaderStageCreateInfo& stageCreateInfo);
    virtual ~Shader();

    void dispose();

    const VkPipelineShaderStageCreateInfo& getStageCreateInfo() const;

private:
    VkDevice vkDevice = nullptr;
    DECLARE_VULKAN_FUNCTION(vkDestroyShaderModule);

    VkPipelineShaderStageCreateInfo stageCreateInfo = {};
};
    
} // namespace rfx
